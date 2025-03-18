use ::std::path::Path;
use sha2::{Digest, Sha256};
use std::fs::{self, File};
use std::io::{Read, Write};
use std::{env, process};

fn usage() {
    println!("Usage:");
    println!("split <file> -s <size> --> size has the format <number><suffix>\nnumber must be an integer and suffix can be b(default), kb, mb or gb");
    println!("unsplit <file>");
}

fn parse_size(size: &str) -> Result<u64, String> {
    let suffixes = [
        ("kb", 1_000),
        ("mb", 1_000_000),
        ("gb", 1_000_000_000),
        ("b", 1),
    ];
    let size = size.to_lowercase();
    let size = if size.ends_with('k') || size.ends_with('m') || size.ends_with('g') {
        size + "b"
    } else {
        size
    };

    for (suffix, multiplier) in &suffixes {
        if size.ends_with(suffix) {
            let num_part = &size[..size.len() - suffix.len()];
            match num_part.parse::<u64>() {
                Ok(num) => {
                    return Ok(num * multiplier);
                }
                Err(err) => {
                    eprintln!("error: invalid size format: {}", err);
                    usage();
                    process::exit(1);
                }
            }
        }
    }

    match size.parse::<u64>() {
        Ok(num) => Ok(num),
        Err(err) => {
            eprintln!("error: invalid size format: {}", err);
            usage();
            process::exit(1);
        }
    }
}

fn calculate_hash(file_path: &Path) -> Result<String, String> {
    let mut file = match File::open(file_path) {
        Ok(file) => file,
        Err(e) => {
            eprintln!("error: cannot open file: {}", e);
            return Err(format!("error: cannot open file: {}", e));
        }
    };

    let mut contents = Vec::new();
    match file.read_to_end(&mut contents) {
        Ok(_) => {}
        Err(e) => {
            eprintln!("error: cannot read file: {}", e);
            return Err(format!("error: cannot read file: {}", e));
        }
    }

    let hash = Sha256::digest(&contents);
    Ok(format!("{:x}", hash))
}

fn split_file(input_file: &str, part_size: u64) {
    let input_path = Path::new(input_file);
    if !input_path.exists() {
        eprintln!("error: input file does not exist.");
        return;
    }

    let original_hash = match calculate_hash(input_path) {
        Ok(hash) => hash,
        Err(err) => {
            eprintln!("{}", err);
            return;
        }
    };

    let hash_file_path = format!("{}.hash", input_file);
    if let Err(err) = fs::write(&hash_file_path, &original_hash) {
        eprintln!("error writing hash file: {}", err);
        return;
    }
    println!("Original file hash saved to: {}", hash_file_path);

    let file_size = match input_path.metadata() {
        Ok(metadata) => metadata.len(),
        Err(err) => {
            eprintln!("error: getting file metadata: {}", err);
            return;
        }
    };

    let total_parts = (file_size + part_size - 1) / part_size;
    let digits = ((total_parts as f64).log10().ceil() as usize).max(4);

    let mut input = match File::open(input_path) {
        Ok(file) => file,
        Err(err) => {
            eprintln!("error: opening file: {}", err);
            return;
        }
    };

    let subfolder = match input_path.file_name() {
        Some(name) => format!("{}.parts", name.to_string_lossy()),
        None => {
            eprintln!("error: could not determine file name.");
            return;
        }
    };

    if Path::new(&subfolder).exists() {
        match fs::remove_dir_all(&subfolder) {
            Ok(()) => {}
            Err(err) => {
                eprintln!("error deleting existing subfolder: {}", err);
                return;
            }
        }
    }

    if let Err(err) = fs::create_dir_all(&subfolder) {
        eprintln!("error creating directory: {}", err);
        return;
    }

    let mut buffer = vec![0u8; part_size as usize];
    let mut part = 1;

    loop {
        let bytes_read = match input.read(&mut buffer) {
            Ok(0) => break,
            Ok(n) => n,
            Err(err) => {
                eprintln!("error reading file: {}", err);
                return;
            }
        };

        let part_name = format!("{}/{}.part{:0digits$}.split", subfolder, input_file, part);

        let mut part_file = match File::create(&part_name) {
            Ok(file) => file,
            Err(err) => {
                eprintln!("error creating file: {}", err);
                return;
            }
        };

        if let Err(err) = part_file.write_all(&buffer[..bytes_read]) {
            eprintln!("error writing to file: {}", err);
            return;
        }

        println!("Created part file: {}, of size {}b", part_name, bytes_read);

        part += 1;
    }
    println!("Subfolder {} contains all parts.", subfolder);
}

fn unsplit_file(output_file: &str) {
    let output_path = Path::new(output_file);
    let subfolder = format!("{}.parts", output_file);

    let subfolder_path = Path::new(&subfolder);
    if !subfolder_path.exists() {
        eprintln!("error: subfolder with parts does not exist.");
        return;
    }

    let mut part_files: Vec<std::path::PathBuf> = Vec::new();

    let entries = match fs::read_dir(subfolder_path) {
        Ok(entries) => entries,
        Err(err) => {
            eprintln!("error: failed to read subfolder: {}", err);
            return;
        }
    };

    for entry in entries {
        match entry {
            Ok(entry) => {
                part_files.push(entry.path());
            }
            Err(err) => {
                eprintln!("error: failed to push part file: {}", err);
                return;
            }
        }
    }

    let mut output = match File::create(output_path) {
        Ok(file) => file,
        Err(err) => {
            eprintln!("error: could not create output file: {}", err);
            return;
        }
    };

    let mut part_num = 1;

    for part_file in part_files {
        let mut part = match File::open(&part_file) {
            Ok(file) => file,
            Err(err) => {
                eprintln!("error: could not open part file: {}", err);
                return;
            }
        };

        let mut buffer = Vec::new();
        match part.read_to_end(&mut buffer) {
            Ok(_) => {}
            Err(err) => {
                eprintln!("error: could not read part file: {}", err);
                return;
            }
        }

        if let Err(err) = output.write_all(&buffer) {
            eprintln!("error: could not write to output file: {}", err);
            return;
        }

        println!("Added part file: {}", part_num);
        part_num += 1;
    }

    println!("Successfully unsplit file: {}", output_file);

    if let Err(err) = fs::remove_dir_all(subfolder_path) {
        eprintln!("error deleting subfolder with parts: {}", err);
    } else {
        println!("Deleted subfolder: {}", subfolder);
    }

    let original_hash_path = format!("{}.hash", output_file);
    let original_hash = match fs::read_to_string(&original_hash_path) {
        Ok(hash) => hash,
        Err(err) => {
            eprintln!("error: missing hash file: {}", err);
            return;
        }
    };

    let unsplit_hash = match calculate_hash(output_path) {
        Ok(hash) => hash,
        Err(err) => {
            eprintln!("{}", err);
            return;
        }
    };

    if unsplit_hash == original_hash {
        println!("Unsplit file has not been corrupted.");
    } else {
        eprintln!("error: unsplit file has been corrupted.");
    }

    if let Err(err) = fs::remove_file(&original_hash_path) {
        eprintln!("error deleting hash file: {}", err);
    } else {
        println!("Deleted hash file: {}", original_hash_path);
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 3 {
        usage();
        process::exit(1);
    }
    match args[1].as_str() {
        "split" => {
            if args.len() != 5 || args[3] != "-s" {
                usage();
                process::exit(1);
            }
            let input_file = &args[2];
            let size = &args[4];

            let part_size = match parse_size(size) {
                Ok(size) => size,
                Err(err) => {
                    eprintln!("{}", err);
                    process::exit(1);
                }
            };

            split_file(input_file, part_size);
        }
        "unsplit" => {
            if args.len() != 3 {
                usage();
                process::exit(1);
            }
            let output_file = &args[2];
            unsplit_file(output_file);
        }
        _ => {
            usage();
            process::exit(1);
        }
    }
}