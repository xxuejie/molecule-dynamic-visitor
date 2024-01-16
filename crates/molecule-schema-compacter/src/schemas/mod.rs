#![allow(dead_code)]

pub mod definitions;

use molecule::prelude::*;
use molecule_codegen::ir;
use std::collections::{HashMap, HashSet};

use definitions as d;

impl From<&str> for d::String {
    fn from(value: &str) -> Self {
        d::StringBuilder::default()
            .extend(value.as_bytes().iter().map(|b| (*b).into()))
            .build()
    }
}

impl From<&ir::ItemDecl> for d::String {
    fn from(value: &ir::ItemDecl) -> Self {
        value.typ.as_str().into()
    }
}

impl From<&ir::Option_> for d::OptionDefinition {
    fn from(value: &ir::Option_) -> Self {
        d::OptionDefinitionBuilder::default()
            .name(value.name.as_str().into())
            .item((&value.item).into())
            .build()
    }
}

impl From<usize> for d::Uint64 {
    fn from(value: usize) -> Self {
        let mut data: [Byte; 8] = Default::default();
        for (i, b) in value.to_le_bytes().into_iter().enumerate() {
            data[i] = b.into();
        }
        d::Uint64Builder::default().set(data).build()
    }
}

impl From<&ir::UnionItemDecl> for d::UnionPair {
    fn from(value: &ir::UnionItemDecl) -> Self {
        d::UnionPairBuilder::default()
            .typ(value.typ.as_str().into())
            .id(value.id.into())
            .build()
    }
}

impl From<&ir::Union> for d::UnionDefinition {
    fn from(value: &ir::Union) -> Self {
        let items = d::UnionPairVecBuilder::default()
            .extend(value.items.iter().map(|decl| decl.into()))
            .build();
        d::UnionDefinitionBuilder::default()
            .name(value.name.as_str().into())
            .items(items)
            .build()
    }
}

impl From<&ir::Array> for d::ArrayDefinition {
    fn from(value: &ir::Array) -> Self {
        d::ArrayDefinitionBuilder::default()
            .name(value.name.as_str().into())
            .item((&value.item).into())
            .item_count(value.item_count.into())
            .build()
    }
}

impl From<&ir::FieldDecl> for d::FieldPair {
    fn from(value: &ir::FieldDecl) -> Self {
        d::FieldPairBuilder::default()
            .name(value.name.as_str().into())
            .typ(value.typ.as_str().into())
            .build()
    }
}

impl From<&ir::Struct> for d::StructDefinition {
    fn from(value: &ir::Struct) -> Self {
        let fields = d::FieldPairVecBuilder::default()
            .extend(value.fields.iter().map(|decl| decl.into()))
            .build();
        d::StructDefinitionBuilder::default()
            .name(value.name.as_str().into())
            .fields(fields)
            .build()
    }
}

impl From<&ir::FixVec> for d::FixvecDefinition {
    fn from(value: &ir::FixVec) -> Self {
        d::FixvecDefinitionBuilder::default()
            .name(value.name.as_str().into())
            .item((&value.item).into())
            .build()
    }
}

impl From<&ir::DynVec> for d::DynvecDefinition {
    fn from(value: &ir::DynVec) -> Self {
        d::DynvecDefinitionBuilder::default()
            .name(value.name.as_str().into())
            .item((&value.item).into())
            .build()
    }
}

impl From<&ir::Table> for d::TableDefinition {
    fn from(value: &ir::Table) -> Self {
        let fields = d::FieldPairVecBuilder::default()
            .extend(value.fields.iter().map(|decl| decl.into()))
            .build();
        d::TableDefinitionBuilder::default()
            .name(value.name.as_str().into())
            .fields(fields)
            .build()
    }
}

impl From<&ir::TopDecl> for d::Definition {
    fn from(value: &ir::TopDecl) -> Self {
        match value {
            ir::TopDecl::Option_(v) => d::DefinitionBuilder::default()
                .set::<d::OptionDefinition>(v.into())
                .build(),
            ir::TopDecl::Union(v) => d::DefinitionBuilder::default()
                .set::<d::UnionDefinition>(v.into())
                .build(),
            ir::TopDecl::Array(v) => d::DefinitionBuilder::default()
                .set::<d::ArrayDefinition>(v.into())
                .build(),
            ir::TopDecl::Struct(v) => d::DefinitionBuilder::default()
                .set::<d::StructDefinition>(v.into())
                .build(),
            ir::TopDecl::FixVec(v) => d::DefinitionBuilder::default()
                .set::<d::FixvecDefinition>(v.into())
                .build(),
            ir::TopDecl::DynVec(v) => d::DefinitionBuilder::default()
                .set::<d::DynvecDefinition>(v.into())
                .build(),
            ir::TopDecl::Table(v) => d::DefinitionBuilder::default()
                .set::<d::TableDefinition>(v.into())
                .build(),
        }
    }
}

pub fn build_compact_definitions<'a, T: Iterator<Item = &'a ir::TopDecl>>(
    syntax_version: usize,
    top_level_type: &str,
    top_decls: T,
) -> d::Definitions {
    let decls = d::DefinitionVecBuilder::default()
        .extend(top_decls.map(|decl| decl.into()))
        .build();
    d::DefinitionsBuilder::default()
        .syntax_version(syntax_version.into())
        .top_level_type(top_level_type.into())
        .definitions(decls)
        .build()
}

pub fn decl_name(decl: &ir::TopDecl) -> String {
    match decl {
        ir::TopDecl::Option_(v) => &v.name,
        ir::TopDecl::Union(v) => &v.name,
        ir::TopDecl::Array(v) => &v.name,
        ir::TopDecl::Struct(v) => &v.name,
        ir::TopDecl::FixVec(v) => &v.name,
        ir::TopDecl::DynVec(v) => &v.name,
        ir::TopDecl::Table(v) => &v.name,
    }
    .clone()
}

// Fetch all used types for a top level declaration
pub fn decl_child_types(
    top_level_type: &String,
    decls: &HashMap<String, ir::TopDecl>,
) -> HashSet<String> {
    let mut result = HashSet::default();
    rec_decl_child_types(&mut result, top_level_type, decls);
    result
}

fn rec_decl_child_types(
    result: &mut HashSet<String>,
    decl_name: &String,
    decls: &HashMap<String, ir::TopDecl>,
) {
    result.insert(decl_name.clone());
    if decl_name == "byte" {
        return;
    }
    let decl = decls.get(decl_name).unwrap();

    match decl {
        ir::TopDecl::Option_(v) => {
            rec_decl_child_types(result, &v.item.typ, decls);
        }
        ir::TopDecl::Union(v) => {
            for item in &v.items {
                rec_decl_child_types(result, &item.typ, decls);
            }
        }
        ir::TopDecl::Array(v) => {
            rec_decl_child_types(result, &v.item.typ, decls);
        }
        ir::TopDecl::Struct(v) => {
            for field in &v.fields {
                rec_decl_child_types(result, &field.typ, decls);
            }
        }
        ir::TopDecl::FixVec(v) => {
            rec_decl_child_types(result, &v.item.typ, decls);
        }
        ir::TopDecl::DynVec(v) => {
            rec_decl_child_types(result, &v.item.typ, decls);
        }
        ir::TopDecl::Table(v) => {
            for field in &v.fields {
                rec_decl_child_types(result, &field.typ, decls);
            }
        }
    }
}
