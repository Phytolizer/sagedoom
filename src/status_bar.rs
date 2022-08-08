use crate::video::Patch;
use crate::wad::cache_lump_name;

#[derive(Debug)]
pub(crate) struct State {
    pub(crate) sbar: Option<Patch>,
}

impl State {
    pub(crate) fn new() -> Self {
        Self { sbar: None }
    }
}

pub(crate) fn init(state: &mut crate::state::State) {
    let st = &mut state.status_bar;
    st.sbar = Some(Patch::new(cache_lump_name(&mut state.wad, "STBAR")));
}
