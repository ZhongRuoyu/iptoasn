#include <arpa/inet.h>

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <utility>

struct AutonomousSystem {
    const std::uint32_t ip_start = 0;
    const std::uint32_t ip_end = 0;
    const std::uint32_t asn = 0;
    const std::string country_code = "";
    const std::string description = "";

    bool contains(const std::uint32_t &ip) const {
        return ip_start <= ip && ip <= ip_end;
    }

    bool operator<(const AutonomousSystem &other) const {
        return ip_start < other.ip_start;
    }
};

std::uint32_t ipv4_to_u32(const std::string &ip) {
    struct sockaddr_in sa;
    if (inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr)) != 1) {
        throw std::invalid_argument("Invalid input");
    }
    std::uint32_t u32 = htonl(sa.sin_addr.s_addr);
    return u32;
}

std::string u32_to_ipv4(const std::uint32_t &u32) {
    struct sockaddr_in sa {
        .sin_addr.s_addr = ntohl(u32)
    };
    char buffer[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &(sa.sin_addr), buffer, sizeof buffer) == NULL) {
        throw std::invalid_argument("Invalid input");
    }
    return buffer;
}

auto find_as(const std::set<AutonomousSystem> &lookup, const std::uint32_t &ip)
    -> const std::set<AutonomousSystem>::const_iterator {
    auto entry = lookup.upper_bound((AutonomousSystem){.ip_start = ip});
    if (entry == lookup.cbegin()) {
        return lookup.cend();
    }
    --entry;
    return entry->contains(ip) ? entry : lookup.cend();
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " /path/to/ip2asn-v4-u32.tsv"
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::ifstream ifs(argv[1]);
    if (ifs.fail()) {
        std::cerr << "Cannot open file: " << argv[1] << std::endl;
        exit(EXIT_FAILURE);
    }

    std::set<AutonomousSystem> lookup;
    std::cout << "Reading file... " << std::flush;
    while (!ifs.eof()) {
        std::uint32_t ip_start;
        std::uint32_t ip_end;
        std::uint32_t asn;
        std::string country_code;
        std::string description;
        ifs >> ip_start >> ip_end >> asn >> country_code >> std::ws;
        std::getline(ifs, description);
        lookup.emplace((AutonomousSystem){
            .ip_start = ip_start,
            .ip_end = ip_end,
            .asn = asn,
            .country_code = country_code,
            .description = description,
        });
    }
    std::cout << "finished." << std::endl;
    std::cout << "Read " << lookup.size() << " records." << std::endl;
    std::cout << std::endl;
    ifs.close();

    while (true) {
        std::cout << "Enter your query (empty line to exit): " << std::flush;
        std::string input;
        std::getline(std::cin, input);
        if (input.length() == 0) {
            std::cout << std::endl;
            break;
        }

        std::uint32_t ip;
        try {
            ip = ipv4_to_u32(input);
        } catch (const std::invalid_argument &ex) {
            std::cerr << ex.what() << std::endl;
            std::cout << std::endl;
            continue;
        }

        auto entry = find_as(lookup, ip);
        if (entry != lookup.cend()) {
            std::cout << "ASN: " << entry->asn << std::endl;
            std::cout << "Net Range: " << u32_to_ipv4(entry->ip_start) << " - "
                      << u32_to_ipv4(entry->ip_end) << std::endl;
            std::cout << "Country Code: " << entry->country_code << std::endl;
            std::cout << "Description: " << entry->description << std::endl;
        } else {
            std::cout << "Not found" << std::endl;
        }
        std::cout << std::endl;
    }
}
