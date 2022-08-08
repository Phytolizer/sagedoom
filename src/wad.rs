use byteorder::LittleEndian;
use byteorder::ReadBytesExt;
use std::fs::File;
use std::io::Read;
use std::io::Seek;
use std::io::SeekFrom;
use std::path::Path;
use std::path::PathBuf;
use std::rc::Rc;

pub(crate) struct WadInfo {
    pub(crate) num_lumps: i32,
    pub(crate) info_table_offset: i32,
}

#[derive(Clone)]
pub(crate) struct FileLump {
    pub(crate) file_pos: i32,
    pub(crate) size: i32,
    pub(crate) name: [u8; 8],
}

#[derive(Debug, Clone)]
pub(crate) struct LumpInfo {
    pub(crate) name: String,
    pub(crate) handle: Rc<File>,
    pub(crate) position: usize,
    pub(crate) size: usize,
}

#[derive(Debug)]
pub(crate) struct State {
    pub(crate) lump_cache: Vec<Vec<u8>>,
    pub(crate) lump_info: Vec<LumpInfo>,
    pub(crate) num_lumps: usize,
}

impl State {
    pub(crate) fn new() -> Self {
        Self {
            lump_cache: Vec::new(),
            lump_info: Vec::new(),
            num_lumps: 0,
        }
    }
}

pub(crate) fn init(state: &mut State, filenames: impl AsRef<[PathBuf]>) {
    for filename in filenames.as_ref().iter() {
        add_file(state, filename);
    }

    if state.num_lumps == 0 {
        panic!("no files found");
    }

    state.lump_cache.resize(state.num_lumps, Vec::new());
}

fn add_file(state: &mut State, filename: impl AsRef<Path>) {
    let filename = filename.as_ref();
    let mut handle = match File::open(filename) {
        Ok(handle) => handle,
        Err(_) => {
            println!(" couldn't open {}", filename.display());
            return;
        }
    };
    println!(" adding {}", filename.display());
    let start_lump = state.num_lumps;
    let file_info = if filename
        .extension()
        .map(|e| e.to_str())
        .flatten()
        .map(|e| e.eq_ignore_ascii_case("wad"))
        .unwrap_or(false)
    {
        let mut identification = [0u8; 4];
        handle.read_exact(&mut identification).unwrap();
        if ![b"IWAD", b"PWAD"].contains(&&identification) {
            panic!(
                "Wad file {} doesn't have IWAD or PWAD id",
                filename.display()
            );
        }
        let num_lumps = handle.read_i32::<LittleEndian>().unwrap();
        let info_table_offset = handle.read_i32::<LittleEndian>().unwrap();
        let header = WadInfo {
            num_lumps,
            info_table_offset,
        };
        let num_lumps = header.num_lumps.try_into().unwrap();
        let mut file_info = Vec::new();
        file_info.reserve(num_lumps);
        handle
            .seek(SeekFrom::Start(
                header.info_table_offset.try_into().unwrap(),
            ))
            .unwrap();
        for _ in 0..(header.num_lumps) {
            let file_pos = handle.read_i32::<LittleEndian>().unwrap();
            let size = handle.read_i32::<LittleEndian>().unwrap();
            let mut name = [0u8; 8];
            handle.read_exact(&mut name).unwrap();
            file_info.push(FileLump {
                file_pos,
                size,
                name,
            });
        }
        state.num_lumps += num_lumps;
        file_info
    } else {
        let file_info = vec![FileLump {
            file_pos: 0,
            size: 0,
            name: filename
                .file_stem()
                .unwrap()
                .to_string_lossy()
                .as_bytes()
                .try_into()
                .unwrap(),
        }];
        state.num_lumps += 1;
        file_info
    };

    state.lump_info.extend(
        std::iter::repeat(LumpInfo {
            name: String::new(),
            handle: Rc::new(handle),
            position: 0,
            size: 0,
        })
        .take(state.num_lumps - start_lump),
    );

    for (info, lump) in file_info
        .into_iter()
        .zip(state.lump_info.iter_mut().skip(start_lump))
    {
        let name_end = info.name.iter().position(|c| *c == 0).unwrap_or(8);
        lump.name = String::from_utf8_lossy(&info.name[0..name_end]).to_string();
        lump.position = info.file_pos.try_into().unwrap();
        lump.size = info.size.try_into().unwrap();
    }
}

pub(crate) fn check_num_for_name(state: &State, name: impl AsRef<str>) -> Option<usize> {
    let name = name.as_ref();

    for (i, lump) in state.lump_info.iter().enumerate() {
        if lump.name.eq_ignore_ascii_case(name) {
            return Some(i);
        }
    }
    None
}
