// Needed for `box State::new()`.
#![feature(box_syntax)]

use std::env;
use std::path::PathBuf;
use std::time::Duration;
use std::time::Instant;

use args::check_parm;
use args::parm_exists;
use const_format::concatcp;
use glium::glutin::ContextBuilder;
use glium::implement_vertex;
use glium::index::IndexBuffer;
use glium::texture::MipmapsOption;
use glium::uniform;
use glium::uniforms::MagnifySamplerFilter;
use glium::uniforms::MinifySamplerFilter;
use glium::Display;
use glium::DrawParameters;
use glium::Program;
use glium::Surface;
use glium::Texture2d;
use glium::VertexBuffer;
use screen::SCREENHEIGHT;
use screen::SCREENWIDTH;
use state::GameMode;
use state::State;
use winit::dpi::PhysicalSize;
use winit::event::Event;
use winit::event::StartCause;
use winit::event::WindowEvent;
use winit::event_loop::EventLoop;
use winit::window::Fullscreen;
use winit::window::WindowBuilder;

mod args;
mod screen;
mod state;
mod status_bar;
mod video;
mod wad;

const VERSION_MAJOR: usize = 1;
const VERSION_MINOR: usize = 10;
const VERSION: &str = concatcp!(VERSION_MAJOR, ".", VERSION_MINOR);

#[derive(Clone, Copy)]
struct Vertex {
    position: [f32; 2],
    tex_coord: [f32; 2],
}

fn identify_version(game_mode: &mut GameMode, wad_files: &mut Vec<PathBuf>) {
    let doom_wad_dir = PathBuf::from(env::var("DOOMWADDIR").unwrap_or_else(|_| ".".to_string()));
    let doom2_wad = doom_wad_dir.join("doom2.wad");
    let doomu_wad = doom_wad_dir.join("doomu.wad");
    let doom_wad = doom_wad_dir.join("doom.wad");
    let doom1_wad = doom_wad_dir.join("doom1.wad");
    let plutonia_wad = doom_wad_dir.join("plutonia.wad");
    let tnt_wad = doom_wad_dir.join("tnt.wad");

    if doom2_wad.exists() {
        *game_mode = GameMode::Commercial;
        wad_files.push(doom2_wad);
    } else if plutonia_wad.exists() {
        *game_mode = GameMode::Commercial;
        wad_files.push(plutonia_wad);
    } else if doomu_wad.exists() {
        *game_mode = GameMode::Retail;
        wad_files.push(doomu_wad);
    } else if doom_wad.exists() {
        *game_mode = GameMode::Registered;
        wad_files.push(doom_wad);
    } else if doom1_wad.exists() {
        *game_mode = GameMode::Shareware;
        wad_files.push(doom1_wad);
    } else if tnt_wad.exists() {
        *game_mode = GameMode::Shareware;
        wad_files.push(tnt_wad);
    } else {
        println!("Game mode indeterminate.");
        *game_mode = GameMode::Undetermined;
    }
}

fn print_centered(terminal_size: (u16, u16), text: impl AsRef<str>) {
    let (width, _) = terminal_size;
    let text = text.as_ref();
    println!("{:^width$}", text, width = width.try_into().unwrap());
}

fn print_centered_fill(terminal_size: (u16, u16), text: impl AsRef<str>, fill: char) {
    let (width, _) = terminal_size;
    let width = width as usize;
    let text = text.as_ref();
    let pad_left = (width - text.len()) / 2;
    let pad_right = width - text.len() - pad_left;
    let fill_left = (0..pad_left).map(|_| fill).collect::<String>();
    let fill_right = (0..pad_right).map(|_| fill).collect::<String>();
    println!(
        "{:>pad_left$}{}{:<pad_right$}",
        fill_left,
        text,
        fill_right,
        pad_left = pad_left.try_into().unwrap(),
        pad_right = pad_right.try_into().unwrap()
    );
}

fn main() {
    let mut state = box State::new();

    let mut wad_files = Vec::new();
    identify_version(&mut state.game_mode, &mut wad_files);

    state.no_monsters = parm_exists(&state.args, "-nomonsters");
    state.respawn = parm_exists(&state.args, "-respawn");
    state.fast = parm_exists(&state.args, "-fast");
    state.dev_parm = parm_exists(&state.args, "-devparm");

    let title = match state.game_mode {
        GameMode::Retail => format!("The Ultimate DOOM Startup v{VERSION}"),
        GameMode::Shareware => format!("DOOM Shareware Startup v{VERSION}"),
        GameMode::Registered => format!("DOOM Registered Startup v{VERSION}"),
        GameMode::Commercial => format!("DOOM 2: Hell on Earth v{VERSION}"),
        GameMode::Undetermined => format!("Public DOOM - v{VERSION}"),
    };

    print_centered(state.terminal_size, title);

    if state.dev_parm {
        println!("Development mode ON.");
    }

    if let Some(p) = check_parm(&state.args, "-turbo") {
        let scale = if p < state.args.len() - 1 {
            state.args[p + 1].parse::<isize>().unwrap().clamp(10, 400)
        } else {
            200
        };
        println!("turbo scale: {scale}%");
        state.forward_move[0] = state.forward_move[0] * scale / 100;
        state.forward_move[1] = state.forward_move[1] * scale / 100;
        state.side_move[0] = state.side_move[0] * scale / 100;
        state.side_move[1] = state.side_move[1] * scale / 100;
    }

    if let Some(mut p) = check_parm(&state.args, "-file") {
        state.modified_game = true;
        p += 1;
        while p < state.args.len() && !state.args[p].starts_with('-') {
            wad_files.push(PathBuf::from(&state.args[p]));
            p += 1;
        }
    }

    wad::init(&mut state.wad, &wad_files);

    if state.modified_game {
        const NAMES: &[&str] = &[
            "e2m1", "e2m2", "e2m3", "e2m4", "e2m5", "e2m6", "e2m7", "e2m8", "e2m9", "e3m1", "e3m2",
            "e3m3", "e3m4", "e3m5", "e3m6", "e3m7", "e3m8", "e3m9", "dphoof", "bfgga0", "heada1",
            "cybra1", "spida1d1",
        ];
        if state.game_mode == GameMode::Shareware {
            panic!("You cannot -file with the shareware version. Register!");
        }

        if state.game_mode == GameMode::Registered {
            if !NAMES
                .iter()
                .all(|n| wad::check_num_for_name(&state.wad, n).is_some())
            {
                panic!("This is not the registered version.");
            }
        }

        print_centered_fill(state.terminal_size, "", '=');
        print_centered(
            state.terminal_size,
            "ATTENTION:  This version of DOOM has been modified.  If you would like to",
        );
        print_centered(
            state.terminal_size,
            "get a copy of the original game, call 1-800-IDGAMES or see the readme file.",
        );
        print_centered(
            state.terminal_size,
            "You will not receive technical support for modified games.",
        );
        print_centered_fill(state.terminal_size, "", '=');
    }

    match state.game_mode {
        GameMode::Shareware | GameMode::Undetermined => {
            print_centered_fill(state.terminal_size, "", '=');
            print_centered(state.terminal_size, "Shareware!");
            print_centered_fill(state.terminal_size, "", '=');
        }
        GameMode::Registered | GameMode::Retail | GameMode::Commercial => {
            print_centered_fill(state.terminal_size, "", '=');
            print_centered(
                state.terminal_size,
                "Commercial product - do not distribute!",
            );
            print_centered(
                state.terminal_size,
                "Please report software piracy to the SPA: 1-800-388-PIR8",
            );
            print_centered_fill(state.terminal_size, "", '=');
        }
    }

    println!("ST_Init: Init status bar.");
    status_bar::init(&mut state);

    let event_loop = EventLoop::new();
    let wb = WindowBuilder::new().with_fullscreen(Some(Fullscreen::Borderless(None)));
    let cb = ContextBuilder::new();
    let display = Display::new(wb, cb, &event_loop).unwrap();

    implement_vertex!(Vertex, position, tex_coord);

    let shape = determine_shape(display.gl_window().window().inner_size());
    let vertex_buffer = VertexBuffer::new(&display, &shape).unwrap();
    let indices = IndexBuffer::new(
        &display,
        glium::index::PrimitiveType::TrianglesList,
        &[0u16, 1, 2, 0, 2, 3],
    )
    .unwrap();
    let tex = Texture2d::empty_with_mipmaps(
        &display,
        MipmapsOption::NoMipmap,
        SCREENWIDTH as u32,
        SCREENHEIGHT as u32,
    )
    .unwrap();

    const VERTEX_SRC: &str = "
        #version 330 core
        in vec2 position;
        in vec2 tex_coord;

        out vec2 pass_tex_coord;

        void main() {
            gl_Position = vec4(position, 0.0, 1.0);
            pass_tex_coord = tex_coord;
        }
    ";

    // draw a texture over the whole screen
    const FRAGMENT_SRC: &str = "
        #version 330 core
        in vec2 pass_tex_coord;
        out vec4 color;
        uniform sampler2D tex;
        void main() {
            color = texture(tex, pass_tex_coord);
        }
    ";

    let program = Program::from_source(&display, VERTEX_SRC, FRAGMENT_SRC, None).unwrap();

    for x in 0..SCREENWIDTH {
        for y in 0..SCREENHEIGHT {
            let u = x as f32 / SCREENWIDTH as f32 * 255.999;
            let v = y as f32 / SCREENHEIGHT as f32 * 255.999;
            state.screens[0].draw(x, y, [u as u8, v as u8, 255]);
        }
    }

    event_loop.run(move |event, _, control_flow| {
        const FPS: usize = 35;
        const FRAME_TIME: f64 = 1.0 / FPS as f64;
        let next_frame_time = Instant::now() + Duration::from_secs_f64(FRAME_TIME);
        control_flow.set_wait_until(next_frame_time);
        match event {
            Event::WindowEvent {
                event: WindowEvent::CloseRequested,
                ..
            } => {
                control_flow.set_exit();
                return;
            }
            Event::NewEvents(cause) => match cause {
                StartCause::ResumeTimeReached { .. } => {}
                StartCause::Init => {}
                _ => return,
            },
            Event::MainEventsCleared => {
                tex.write(
                    glium::Rect {
                        left: 0,
                        bottom: 0,
                        width: SCREENWIDTH as u32,
                        height: SCREENHEIGHT as u32,
                    },
                    &state.screens[0],
                );
                status_bar::drawer(&mut state);
                let mut target = display.draw();
                target
                    .draw(
                        &vertex_buffer,
                        &indices,
                        &program,
                        &uniform! {
                            tex: tex.sampled()
                                .magnify_filter(MagnifySamplerFilter::Nearest)
                                .minify_filter(MinifySamplerFilter::Nearest)
                        },
                        &DrawParameters::default(),
                    )
                    .unwrap();
                target.finish().unwrap();
            }
            Event::RedrawRequested(_) => {}
            _ => {}
        }
    })
}

fn determine_shape(inner_size: PhysicalSize<u32>) -> Vec<Vertex> {
    // scale SCREENWIDTH*SCREENHEIGHT rect to fit within inner_size
    let vert_scale = inner_size.width as f32 / SCREENWIDTH as f32;
    let horiz_scale = inner_size.height as f32 / SCREENHEIGHT as f32;
    let scale = vert_scale.min(horiz_scale);
    let (width, height) = (SCREENWIDTH as f32 * scale, SCREENHEIGHT as f32 * scale);
    let offset_x = (inner_size.width as f32 - width) / 2.0;
    let offset_y = (inner_size.height as f32 - height) / 2.0;
    // now scale back to OpenGL coordinates
    let offset_x_scaled = offset_x / inner_size.width as f32 * 2.0;
    let offset_y_scaled = offset_y / inner_size.height as f32 * 2.0;
    let max_x = 1.0 - offset_x_scaled;
    let max_y = 1.0 - offset_y_scaled;

    vec![
        Vertex {
            position: [-max_x, -max_y],
            tex_coord: [0.0, 0.0],
        },
        Vertex {
            position: [max_x, -max_y],
            tex_coord: [1.0, 0.0],
        },
        Vertex {
            position: [max_x, max_y],
            tex_coord: [1.0, 1.0],
        },
        Vertex {
            position: [-max_x, max_y],
            tex_coord: [0.0, 1.0],
        },
    ]
}
