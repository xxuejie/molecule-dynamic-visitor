mod schemas;

use crate::schemas::{build_compact_definitions, decl_child_types, decl_name};
use clap::{command, Arg};
use molecule::prelude::*;
use molecule_codegen::ir::{Ir, TopDecl};
use std::collections::HashMap;

const EXPECTED_SYNTAX_VERSION: usize = 1;

fn main() {
    let matches = command!()
        .arg(
            Arg::new("input-files")
                .short('i')
                .long("input-files")
                .help("Schema intermediate files(in JSON format), use ',' to separate multiple schema files")
                .required(true)
                .value_delimiter(','),
        )
        .arg(
            Arg::new("top-level-type")
                .short('t')
                .long("top-level-type")
                .help("Top level type name")
                .required(true),
        )
        .arg(
            Arg::new("output-file")
                .short('o')
                .long("output-file")
                .help("Output file")
                .required(true),
        )
        .get_matches();

    let builtins_bytes = include_bytes!("schemas/builtins.json");
    let builtins_ir: Ir =
        serde_json::from_slice(&builtins_bytes[..]).expect("deserialize builtins");
    assert_eq!(
        builtins_ir.syntax_version.version(),
        EXPECTED_SYNTAX_VERSION,
        "Mismatched syntax version!"
    );
    let builtin_decls: HashMap<String, TopDecl> = builtins_ir
        .decls
        .into_iter()
        .map(|decl| (decl_name(&decl), decl))
        .collect();

    let input_files = matches
        .get_many::<String>("input-files")
        .expect("cli input files")
        .collect::<Vec<_>>();
    let output_file = matches
        .get_one::<String>("output-file")
        .expect("cli output file");
    let top_level_type = matches
        .get_one::<String>("top-level-type")
        .expect("cli top level type");

    let mut combined_top_decls: HashMap<String, TopDecl> = HashMap::default();
    for input_file in input_files {
        let input_data = std::fs::read(input_file).expect("read");
        let input_ir: Ir = match serde_json::from_slice(&input_data) {
            Ok(ir) => ir,
            Err(e) => panic!("Error parsing {}: {}", input_file, e),
        };
        assert_eq!(
            input_ir.syntax_version.version(),
            EXPECTED_SYNTAX_VERSION,
            "Mismatched syntax version for {}",
            input_file,
        );

        for decl in &input_ir.decls {
            let name = decl_name(decl);
            if let Some(builtin_decl) = builtin_decls.get(&name) {
                assert_eq!(
                    builtin_decl, decl,
                    "Bulitin decl {} encountered, but file {} provides a different definition!",
                    name, input_file,
                );
            }
        }

        // Molecule already ensures that the same definition name cannot be
        // used across files, we don't need to detect for duplication here.
        combined_top_decls.extend(
            input_ir
                .decls
                .into_iter()
                .map(|decl| (decl_name(&decl), decl)),
        );
    }

    if !combined_top_decls.contains_key(top_level_type) {
        panic!(
            "Top level type {} does not exist in provided schemas!",
            top_level_type
        );
    }

    // Here we perform a tree-shaking operation to remove all types
    // that are not reachable from top level type
    let used_types = decl_child_types(top_level_type, &combined_top_decls);
    combined_top_decls.retain(|name, _| used_types.contains(name));

    let sorted_top_decls: Vec<_> = {
        let mut decl_pairs: Vec<_> = combined_top_decls.into_iter().collect();
        decl_pairs.sort_by(|(k1, _), (k2, _)| k1.cmp(k2));
        decl_pairs.into_iter().map(|(_, v)| v).collect()
    };

    let compact_definitions = build_compact_definitions(
        EXPECTED_SYNTAX_VERSION,
        top_level_type.as_str(),
        sorted_top_decls.iter(),
    );

    std::fs::write(output_file, compact_definitions.as_slice()).expect("write");
}
