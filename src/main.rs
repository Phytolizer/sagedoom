// Needed for `box State::new()`.
#![feature(box_syntax)]

use std::env;
use std::path::PathBuf;

use args::check_parm;
use args::parm_exists;
use const_format::concatcp;
use state::GameMode;
use state::State;

mod args;
mod state;
mod wad;

const VERSION_MAJOR: usize = 1;
const VERSION_MINOR: usize = 10;
const VERSION: &str = concatcp!(VERSION_MAJOR, ".", VERSION_MINOR);

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

    match state.game_mode {
        GameMode::Retail => {
            print_centered(
                state.terminal_size,
                format!("The Ultimate DOOM Startup v{VERSION}"),
            );
        }
        GameMode::Shareware => {
            print_centered(
                state.terminal_size,
                format!("DOOM Shareware Startup v{VERSION}"),
            );
        }
        GameMode::Registered => {
            print_centered(
                state.terminal_size,
                format!("DOOM Registered Startup v{VERSION}"),
            );
        }
        GameMode::Commercial => {
            print_centered(
                state.terminal_size,
                format!("DOOM 2: Hell on Earth v{VERSION}"),
            );
        }
        GameMode::Undetermined => {
            print_centered(state.terminal_size, format!("Public DOOM - v{VERSION}"));
        }
    }

    if state.dev_parm {
        println!("Development mode ON.");
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
}
