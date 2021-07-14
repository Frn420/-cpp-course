#include "encstrset.h"

#include <bits/stdc++.h>

using namespace std;

using identifier_t = unsigned long;
using value_t = string;
using debug_message_t = string;
using value_argument_t = const char *;
using key_argument_t = const char *;
using set_t = unordered_set<value_t>;
using map_t = unordered_map<identifier_t, set_t>;

namespace {

#ifndef NDEBUG
constexpr bool DEBUG = true;
#else
constexpr bool DEBUG = false;
#endif

map_t &map_of_sets() {
    static map_t map_of_sets;
    return map_of_sets;
}

identifier_t &number_of_created_sets() {
    static identifier_t number_of_created_sets = 0;
    return number_of_created_sets;
}

void debug_info(const debug_message_t &s) {
    if (DEBUG)
        cerr << s << endl;
}

debug_message_t string_to_hex(const value_t &cypher) {
    stringstream stream;
    for (auto c : cypher)
        stream << setw(2) << setfill('0') << uppercase << hex
               << static_cast<int>(static_cast<unsigned char>(c)) << ' ';

    auto hex_string = stream.str();
    if (not hex_string.empty())
        hex_string.pop_back();

    return hex_string;
}

value_t cyphering(value_argument_t value, key_argument_t key) {
    value_t cypher = value;
    size_t value_length = !key ? strlen(value) : (size_t)0;
    size_t key_length = !key ? strlen(key) : (size_t)0;

    if (key_length == 0)
        return cypher;

    for (size_t i = 0; i < value_length; i++)
        cypher[i] = (value[i] ^ key[i % key_length]);

    return cypher;
}

bool exists_in_map(const map_t::iterator &set_pointer) {
    return set_pointer != map_of_sets().end();
}

bool exists_in_set(const set_t::iterator &element_pointer,
                   const map_t::iterator &set_pointer) {
    return element_pointer != set_pointer->second.end();
}

}  // namespace

namespace jnp1 {

identifier_t encstrset_new() {
    debug_info("encstrset_new()");

    set_t new_set;
    map_of_sets()[number_of_created_sets()] = new_set;

    debug_info("encstrset_new: set #" + to_string(number_of_created_sets()) +
               " created");

    return number_of_created_sets()++;
}

void encstrset_delete(identifier_t id) {
    debug_info("encstrset_delete(" + to_string(id) + ")");

    auto set_pointer = map_of_sets().find(id);

    if (exists_in_map(set_pointer)) {
        map_of_sets().erase(set_pointer);
        debug_info("encstrset_delete: set #" + to_string(id) + " deleted");
    }
    else
        debug_info("encstrset_delete: set #" + to_string(id) +
                   " does not exist");
}

size_t encstrset_size(identifier_t id) {
    debug_info("encstrset_size(" + to_string(id) + ")");

    auto set_pointer = map_of_sets().find(id);

    if (exists_in_map(set_pointer)) {
        size_t size = set_pointer->second.size();

        debug_info("encstrset_size: set #" + to_string(id) + " contains " +
                   to_string(size) + " element(s)");

        return size;
    }
    else {
        debug_info("encstrset_size: set #" + to_string(id) + " does not exist");

        return 0;
    }
}

bool encstrset_insert(identifier_t id, value_argument_t value,
                      key_argument_t key) {
    debug_info("encstrset_insert(" + to_string(id) + ", \"" +
               (value ? string(value) : "NULL") + "\", \"" +
               (key ? string(key) : "NULL") + "\")");

    if (!value) {
        debug_info("encstrset_insert: invalid value (NULL)");

        return false;
    }

    auto set_pointer = map_of_sets().find(id);

    if (not exists_in_map(set_pointer)) {
        debug_info("encstrset_insert: set #" + to_string(id) +
                   " does not exist");

        return false;
    }

    auto cypher = cyphering(value, key);
    auto element_pointer = set_pointer->second.find(cypher);

    if (not exists_in_set(element_pointer, set_pointer)) {
        set_pointer->second.insert(cypher);

        debug_info("encstrset_insert: set #" + to_string(id) + ", cypher \"" +
                   string_to_hex(cypher) + "\" inserted");

        return true;
    }
    else {
        debug_info("encstrset_insert: set #" + to_string(id) + ", cypher \"" +
                   string_to_hex(cypher) + "\" was already present");

        return false;
    }
}

bool encstrset_remove(identifier_t id, value_argument_t value,
                      key_argument_t key) {
    debug_info("encstrset_remove(" + to_string(id) + ", \"" +
               (value ? string(value) : "NULL") + "\", \"" +
               (key ? string(key) : "NULL") + "\")");

    if (!value) {
        debug_info("encstrset_remove: invalid value (NULL)");

        return false;
    }

    auto set_pointer = map_of_sets().find(id);

    if (not exists_in_map(set_pointer)) {
        debug_info("encstrset_remove: set #" + to_string(id) +
                   " does not exist");

        return false;
    }

    auto cypher = cyphering(value, key);
    auto element_pointer = set_pointer->second.find(cypher);

    if (exists_in_set(element_pointer, set_pointer)) {
        set_pointer->second.erase(element_pointer);

        debug_info("encstrset_remove: set #" + to_string(id) + ", cypher \"" +
                   string_to_hex(cypher) + "\" removed");

        return true;
    }
    else {
        debug_info("encstrset_remove: set #" + to_string(id) + ", cypher \"" +
                   string_to_hex(cypher) + "\" was not present");

        return false;
    }
}

bool encstrset_test(identifier_t id, value_argument_t value,
                    key_argument_t key) {
    debug_info("encstrset_test(" + to_string(id) + ", \"" +
               (value ? string(value) : "NULL") + "\", \"" +
               (key ? string(key) : "NULL") + "\")");

    if (!value) {
        debug_info("encstrset_test: invalid value (NULL)");

        return false;
    }

    auto set_pointer = map_of_sets().find(id);

    if (not exists_in_map(set_pointer)) {
        debug_info("encstrset_test: set #" + to_string(id) + " does not exist");

        return false;
    }

    auto cypher = cyphering(value, key);
    auto element_pointer = set_pointer->second.find(cypher);

    if (exists_in_set(element_pointer, set_pointer)) {
        debug_info("encstrset_test: set #" + to_string(id) + ", cypher \"" +
                   string_to_hex(cypher) + "\" is present");

        return true;
    }
    else {
        debug_info("encstrset_test: set #" + to_string(id) + ", cypher \"" +
                   string_to_hex(cypher) + "\" is not present");

        return false;
    }
}

void encstrset_clear(identifier_t id) {
    debug_info("encstrset_clear(" + to_string(id) + ")");

    auto set_pointer = map_of_sets().find(id);

    if (exists_in_map(set_pointer)) {
        set_pointer->second.clear();
        debug_info("encstrset_clear: set #" + to_string(id) + " cleared");
    }
    else {
        debug_info("encstrset_clear: set #" + to_string(id) +
                   " does not exist");
    }
}

void encstrset_copy(identifier_t src_id, identifier_t dst_id) {
    debug_info("encstrset_copy(" + to_string(src_id) + ", " +
               to_string(dst_id) + ")");

    auto src_set_pointer = map_of_sets().find(src_id);
    auto dst_set_pointer = map_of_sets().find(dst_id);

    if (not exists_in_map(src_set_pointer)) {
        debug_info("encstrset_copy: set #" + to_string(src_id) +
                   " does not exist");

        return;
    }
    if (not exists_in_map(dst_set_pointer)) {
        debug_info("encstrset_copy: set #" + to_string(dst_id) +
                   " does not exist");

        return;
    }

    for (const auto &element : src_set_pointer->second) {
        auto element_pointer = dst_set_pointer->second.find(element);

        if (not exists_in_set(element_pointer, dst_set_pointer)) {
            dst_set_pointer->second.insert(element);
            debug_info("encstrset_copy: cypher \"" + string_to_hex(element) +
                       "\" copied from set #" + to_string(src_id) +
                       " to set #" + to_string(dst_id));
        }
        else {
            debug_info("encstrset_copy: copied cypher \"" +
                       string_to_hex(element) +
                       "\" was already present in set #" + to_string(dst_id));
        }
    }
}

}  // namespace jnp1