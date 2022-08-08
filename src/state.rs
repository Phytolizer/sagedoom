use crossterm::terminal;

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
    pub(crate) terminal_size: (u16, u16),
}

impl State {
    pub(crate) fn new() -> Self {
        Self {
            wad: crate::wad::State::new(),
            game_mode: GameMode::Undetermined,
            terminal_size: terminal::size().unwrap_or((1, 1)),
        }
    }
}
