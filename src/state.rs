use std::path::PathBuf;

#[derive(Debug)]
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
    pub(crate) wad_files: Vec<PathBuf>,
}

impl State {
    pub(crate) fn new() -> Self {
        Self {
            wad: crate::wad::State::new(),
            game_mode: GameMode::Undetermined,
            wad_files: Vec::new(),
        }
    }
}
