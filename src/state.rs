use std::env;

use crossterm::terminal;

#[derive(Debug, PartialEq)]
pub(crate) enum GameMode {
    Commercial,
    Retail,
    Registered,
    Shareware,
    Undetermined,
}

#[derive(Debug)]
pub(crate) struct State {
    pub(crate) wad: crate::wad::State,
    pub(crate) game_mode: GameMode,
    pub(crate) terminal_size: (u16, u16),
    pub(crate) args: Vec<String>,
    pub(crate) modified_game: bool,
    pub(crate) no_monsters: bool,
    pub(crate) respawn: bool,
    pub(crate) fast: bool,
    pub(crate) dev_parm: bool,
    pub(crate) forward_move: [isize; 2],
    pub(crate) side_move: [isize; 2],
}

impl State {
    pub(crate) fn new() -> Self {
        Self {
            wad: crate::wad::State::new(),
            game_mode: GameMode::Undetermined,
            terminal_size: terminal::size().unwrap_or((1, 1)),
            args: env::args().collect(),
            modified_game: false,
            no_monsters: false,
            respawn: false,
            fast: false,
            dev_parm: false,
            forward_move: [0x19, 0x32],
            side_move: [0x18, 0x28],
        }
    }
}
