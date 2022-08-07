// Needed for `box State::new()`.
#![feature(box_syntax)]

use state::State;

mod state;
mod wad;

fn main() {
    let mut state = box State::new();

    wad::init(&mut state, vec![]);

    dbg!(state);
}
