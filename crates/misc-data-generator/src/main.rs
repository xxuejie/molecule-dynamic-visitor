mod schemas;

use crate::schemas::misc;
use ckb_sdk::types::{Address, AddressPayload, NetworkType};
use ckb_types::prelude::Pack;
use clap::{command, Arg};
use molecule::prelude::*;
use rand::{rngs::StdRng, Rng, SeedableRng};

// Tweak this for a different environment
const NETWORK: NetworkType = NetworkType::Mainnet;

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

    let integer: u64 = rng.gen();
    let hash = random_byte32(&mut rng);
    let string = rand_utf8::rand_utf8(&mut rng, 64);
    let address = random_address(&mut rng);

    println!("Integer: {}", integer);
    println!("Hash: {:#x}", hash);
    println!("String: {}", string);
    println!("Address: {}", to_address(address.clone()));

    let packed_integer = {
        let mut data = [Byte::new(0); 8];
        for (i, v) in integer.to_le_bytes().into_iter().enumerate() {
            data[i] = Byte::new(v);
        }
        misc::Uint64Builder::default().set(data).build()
    };

    let packed_string = misc::StringBuilder::default()
        .extend(string.as_bytes().iter().map(|b| Byte::new(*b)))
        .build();

    let combined = misc::MiscBuilder::default()
        .integer(packed_integer)
        .hash(hash)
        .string(packed_string)
        .addr(address)
        .build();

    std::fs::write(output_file, combined.as_slice()).expect("write");
}

fn to_address(a: misc::Address) -> Address {
    let misc::AddressUnion::Script(script) = a.to_enum();
    let hash_type: u8 = script.hash_type().into();
    let code_hash: [u8; 32] = script.code_hash().into();
    let payload = AddressPayload::Full {
        hash_type: hash_type.try_into().unwrap(),
        code_hash: code_hash.pack(),
        args: script.args().as_bytes(),
    };
    Address::new(NETWORK, payload, true)
}

fn random_byte32<R: Rng>(rng: &mut R) -> misc::Byte32 {
    let mut data = [0u8; 32];
    rng.fill(&mut data);
    let mut data2 = [Byte::new(0); 32];
    for i in 0..32 {
        data2[i] = Byte::new(data[i]);
    }
    misc::Byte32Builder::default().set(data2).build()
}

fn random_bytes<R: Rng>(rng: &mut R) -> misc::Bytes {
    let len = rng.gen_range(0..100);
    let mut data = vec![0u8; len];
    rng.fill(&mut data[..]);
    let mut data2 = vec![Byte::new(0); len];
    for i in 0..len {
        data2[i] = Byte::new(data[i]);
    }
    misc::BytesBuilder::default().set(data2).build()
}

fn random_address<R: Rng>(rng: &mut R) -> misc::Address {
    let script = misc::ScriptBuilder::default()
        .code_hash(random_byte32(rng))
        .hash_type(Byte::new(rng.gen_range(0..2)))
        .args(random_bytes(rng))
        .build();
    misc::AddressBuilder::default()
        .set::<misc::AddressUnion>(script.into())
        .build()
}
