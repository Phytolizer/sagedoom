// Needed for `box State::new()`.
#![feature(box_syntax)]

use std::env;
use std::path::PathBuf;

use const_format::concatcp;
use state::GameMode;
use state::State;

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

fn print_centered(terminal_size: (u16, u16), text: String) {
    let (width, _) = terminal_size;
    println!("{:^width$}", text, width = width.try_into().unwrap());
}

fn main() {
    let mut state = box State::new();

    let mut wad_files = Vec::new();
    identify_version(&mut state.game_mode, &mut wad_files);

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

    wad::init(&mut state.wad, &wad_files);
}
