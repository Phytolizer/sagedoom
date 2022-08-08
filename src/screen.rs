use glium::texture::RawImage2d;
use glium::texture::Texture2dDataSource;

pub(crate) const SCREENWIDTH: usize = 320;
pub(crate) const SCREENHEIGHT: usize = 200;

#[derive(Debug)]
pub(crate) struct Screen([u8; SCREENWIDTH * 3 * SCREENHEIGHT]);

impl Screen {
    pub(crate) fn new() -> Self {
        Self([0; SCREENWIDTH * SCREENHEIGHT * 3])
    }

    pub(crate) fn draw(&mut self, x: usize, y: usize, px: [u8; 3]) {
        if x < SCREENWIDTH && y < SCREENHEIGHT {
            let index = (y * SCREENWIDTH + x) * 3;
            self.0[index..index + 3].copy_from_slice(&px);
        }
    }
}

impl<'a> Texture2dDataSource<'a> for &'a Screen {
    type Data = u8;

    fn into_raw(self) -> RawImage2d<'a, Self::Data> {
        RawImage2d::from_raw_rgb_reversed(&self.0, (SCREENWIDTH as u32, SCREENHEIGHT as u32))
    }
}
