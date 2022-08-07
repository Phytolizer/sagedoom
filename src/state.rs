#[derive(Debug)]
pub(crate) struct State {
    pub(crate) wad: crate::wad::State,
}

impl State {
    pub(crate) fn new() -> Self {
        Self {
            wad: crate::wad::State::new(),
        }
    }
}
