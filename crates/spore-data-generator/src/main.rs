mod schemas;

use crate::schemas::spore;
use clap::{command, Arg};
use molecule::prelude::*;
use rand::{rngs::StdRng, Rng, SeedableRng};

fn main() {
    let matches = command!()
        .arg(
            Arg::new("output-file")
                .short('o')
                .long("output-file")
                .help("Output file")
                .required(true),
        )
        .get_matches();
    let output_file = matches
        .get_one::<String>("output-file")
        .expect("cli output file");

    let seed: u64 = match std::env::var("SEED") {
        Ok(val) => str::parse(&val).expect("parsing number"),
        Err(_) => std::time::SystemTime::now()
            .duration_since(std::time::SystemTime::UNIX_EPOCH)
            .unwrap()
            .as_nanos() as u64,
    };
    println!("Seed: {}", seed);

    let mut rng = StdRng::seed_from_u64(seed);

    let spore_action: spore::SporeAction = match rng.gen_range(0..3) {
        0 => {
            // MintSpore
            let mint = spore::MintSporeBuilder::default()
                .spore_id(random_byte32(&mut rng))
                .to(random_address(&mut rng))
                .data_hash(random_byte32(&mut rng))
                .build();
            println!("Mint spore id: {:#x}", mint.spore_id());
            println!("Mint data hash: {:#x}", mint.data_hash());
            mint.into()
        }
        1 => {
            // TransferSpore
            let transfer = spore::TransferSporeBuilder::default()
                .spore_id(random_byte32(&mut rng))
                .from(random_address(&mut rng))
                .to(random_address(&mut rng))
                .build();
            println!("Transfer spore id: {:#x}", transfer.spore_id());
            transfer.into()
        }
        _ => {
            // BurnSpore
            let burn = spore::BurnSporeBuilder::default()
                .spore_id(random_byte32(&mut rng))
                .from(random_address(&mut rng))
                .build();
            println!("Burn spore id: {:#x}", burn.spore_id());
            burn.into()
        }
    };

    std::fs::write(output_file, spore_action.as_slice()).expect("write");
}

fn random_byte32<R: Rng>(rng: &mut R) -> spore::Byte32 {
    let mut data = [0u8; 32];
    rng.fill(&mut data);
    let mut data2 = [Byte::new(0); 32];
    for i in 0..32 {
        data2[i] = Byte::new(data[i]);
    }
    spore::Byte32Builder::default().set(data2).build()
}

fn random_bytes<R: Rng>(rng: &mut R) -> spore::Bytes {
    let len = rng.gen_range(0..100);
    let mut data = vec![0u8; len];
    rng.fill(&mut data[..]);
    let mut data2 = vec![Byte::new(0); len];
    for i in 0..len {
        data2[i] = Byte::new(data[i]);
    }
    spore::BytesBuilder::default().set(data2).build()
}

fn random_address<R: Rng>(rng: &mut R) -> spore::Address {
    let script = spore::ScriptBuilder::default()
        .code_hash(random_byte32(rng))
        .hash_type(Byte::new(rng.gen_range(0..2)))
        .args(random_bytes(rng))
        .build();
    spore::AddressBuilder::default()
        .set::<spore::AddressUnion>(script.into())
        .build()
}
