use std::io::Cursor;

use byteorder::LittleEndian;
use byteorder::ReadBytesExt;

#[derive(Debug)]
pub(crate) struct Patch {
    pub(crate) width: i16,
    pub(crate) height: i16,
    pub(crate) left_offset: i16,
    pub(crate) top_offset: i16,
    pub(crate) column_offset: [i32; 8],
    pub(crate) data: Vec<u8>,
}

impl Patch {
    pub(crate) fn new(data: &[u8]) -> Self {
        let mut cursor = Cursor::new(data);
        let width = cursor.read_i16::<LittleEndian>().unwrap();
        let height = cursor.read_i16::<LittleEndian>().unwrap();
        let left_offset = cursor.read_i16::<LittleEndian>().unwrap();
        let top_offset = cursor.read_i16::<LittleEndian>().unwrap();
        let mut column_offset = [0i32; 8];
        for i in 0..8 {
            column_offset[i] = cursor.read_i32::<LittleEndian>().unwrap();
        }
        let data = data[cursor.position() as usize..data.len()].to_vec();
        Self {
            width,
            height,
            left_offset,
            top_offset,
            column_offset,
            data,
        }
    }
}
