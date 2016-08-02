/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/message/get_block_transactions.hpp>

#include <initializer_list>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::get_block_transactions::command = "getblocktxn";

get_block_transactions get_block_transactions::factory_from_data(
    const data_chunk& data)
{
    get_block_transactions instance;
    instance.from_data(data);
    return instance;
}

get_block_transactions get_block_transactions::factory_from_data(
    std::istream& stream)
{
    get_block_transactions instance;
    instance.from_data(stream);
    return instance;
}

get_block_transactions get_block_transactions::factory_from_data(
    reader& source)
{
    get_block_transactions instance;
    instance.from_data(source);
    return instance;
}

bool get_block_transactions::is_valid() const
{
    return (block_hash != null_hash);
}

void get_block_transactions::reset()
{
    block_hash = null_hash;
    indexes.clear();
}

bool get_block_transactions::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool get_block_transactions::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool get_block_transactions::from_data(reader& source)
{
    reset();
    block_hash = source.read_hash();
    auto result = static_cast<bool>(source);

    const auto count = source.read_variable_uint_little_endian();
    result &= static_cast<bool>(source);

    for (uint64_t i = 0; (i < count) && result; ++i)
    {
        indexes.push_back(source.read_variable_uint_little_endian());
        result = static_cast<bool>(source);
    }

    if (!result)
        reset();

    return result;
}

data_chunk get_block_transactions::to_data() const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());
    return data;
}

void get_block_transactions::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void get_block_transactions::to_data(writer& sink) const
{
    sink.write_hash(block_hash);
    sink.write_variable_uint_little_endian(indexes.size());
    for (const auto& element: indexes)
        sink.write_variable_uint_little_endian(element);
}

uint64_t get_block_transactions::serialized_size() const
{
    uint64_t size = hash_size + variable_uint_size(indexes.size());

    for (const auto& element: indexes)
        size += variable_uint_size(element);

    return size;
}

} // namspace message
} // namspace libbitcoin
