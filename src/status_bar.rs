use crate::video::Patch;
use crate::video::draw_patch;
use crate::wad::cache_lump_name;

const ST_X: usize = 0;
const ST_X2: usize = 104;
const ST_FX: usize = 143;
const ST_FY: usize = 169;

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

pub(crate) fn drawer(state: &mut crate::state::State) {
    let sbar = state.status_bar.sbar.as_ref().unwrap();
    draw_patch(&mut state.screens[0], ST_X.try_into().unwrap(), 0, sbar);
}
