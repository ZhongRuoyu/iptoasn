#!/bin/bash -e

home=$(cd $(dirname ${BASH_SOURCE[0]}); pwd -P)
mkdir -p $home/data
curl -o - "https://iptoasn.com/data/ip2asn-v4-u32.tsv.gz" | gunzip > $home/data/ip2asn-v4-u32.tsv
