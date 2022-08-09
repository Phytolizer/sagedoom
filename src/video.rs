use std::io::Cursor;
use std::io::Seek;
use std::io::SeekFrom;

use byteorder::LittleEndian;
use byteorder::ReadBytesExt;

use crate::screen::Screen;
use crate::screen::SCREENWIDTH;

#[derive(Debug)]
pub(crate) struct Patch {
    pub(crate) width: i16,
    pub(crate) height: i16,
    pub(crate) left_offset: i16,
    pub(crate) top_offset: i16,
    pub(crate) column_offset: Vec<i32>,
    pub(crate) hdr_size: usize,
    pub(crate) data: Vec<u8>,
}

#[derive(Debug)]
struct Post {
    pub(crate) top_delta: u8,
    pub(crate) length: u8,
}

type Column = Post;

impl Patch {
    pub(crate) fn new(data: &[u8]) -> Self {
        let mut cursor = Cursor::new(data);
        let width = cursor.read_i16::<LittleEndian>().unwrap();
        let height = cursor.read_i16::<LittleEndian>().unwrap();
        let left_offset = cursor.read_i16::<LittleEndian>().unwrap();
        let top_offset = cursor.read_i16::<LittleEndian>().unwrap();
        let mut column_offset = Vec::new();
        for _ in 0..width {
            column_offset.push(cursor.read_i32::<LittleEndian>().unwrap());
        }
        let hdr_size = cursor.position() as usize;
        let data = data[hdr_size..].to_vec();
        Self {
            width,
            height,
            left_offset,
            top_offset,
            column_offset,
            hdr_size,
            data,
        }
    }
}

pub(crate) fn draw_patch(screen: &mut Screen, x: i32, y: i32, patch: &Patch) {
    let y = (y - patch.top_offset as i32) as usize;
    let x = (x - patch.left_offset as i32) as usize;

    let dest_top = &mut screen[(y * SCREENWIDTH + x) as usize..];

    for col in 0..patch.width as usize {
        let column_data = &patch.data[patch.column_offset[col] as usize - patch.hdr_size..];
        let mut cursor = Cursor::new(column_data);
        let mut column = Column {
            top_delta: cursor.read_u8().unwrap(),
            length: cursor.read_u8().unwrap(),
        };
        while column.top_delta != 0xFF {
            // 2 bytes column data + 1 byte pad
            let source = &column_data[cursor.position() as usize + 1..];
            let dest = &mut dest_top[column.top_delta as usize * SCREENWIDTH..];
            let count = column.length as usize;
            for i in 0..count {
                dest[i * SCREENWIDTH] = source[i];
            }
            // 1 byte pad
            cursor.seek(SeekFrom::Current(1)).unwrap();
            column = Column {
                top_delta: cursor.read_u8().unwrap(),
                length: cursor.read_u8().unwrap(),
            };
        }
    }
}
