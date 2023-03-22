#ifndef ADDRESS_H__
#define ADDRESS_H__

#include <Include.h>

/// Sometimes you need different sizes than your architecture
/// For example 64 bit addresses on 32 bit programs
template <typename ptr_type = uintptr_t>
struct address_base_t
{
    /// <summary>
    /// Inner pointer
    /// </summary>
    ptr_type m_ptr;

    /// <summary>
    /// Creates a NULL address object
    /// </summary>
    address_base_t() :
        m_ptr{} { }

    /// <summary>
    /// Creates an address object with the given pointer
    /// </summary>
    /// <param name="ptr">The address on which the object will be based on</param>
    address_base_t(ptr_type ptr) :
        m_ptr(ptr) { };

    /// <summary>
    /// Creates an address object with the given pointer
    /// </summary>
    /// <param name="ptr">The address on which the object will be based on</param>
    address_base_t(ptr_type *ptr) :
        m_ptr(ptr_type(ptr)) { };

    /// <summary>
    /// Creates an address object with the given pointer
    /// </summary>
    /// <param name="ptr">The address on which the object will be based on</param>
    address_base_t(void *ptr) :
        m_ptr(ptr_type(ptr)) { };

    /// <summary>
    /// Creates an address object with the given pointer
    /// </summary>
    /// <param name="ptr">The address on which the object will be based on</param>
    address_base_t(const void *ptr) :
        m_ptr(ptr_type(ptr)) { };

    /// <summary>
    /// Destroys the address object
    /// </summary>
    ~address_base_t() = default;

    /// <summary>
    /// Whenever an address object is being passed into a function but it requires
    /// a uintptr this function will be called
    /// </summary>
    inline operator ptr_type() const { return m_ptr; }

    /// <summary>
    /// Whenever an address object is being passed into a function but it requires
    /// a void* this function will be called
    /// </summary>
    inline operator void *() { return reinterpret_cast<void *>(m_ptr); }

    /// <summary>
    /// Returns the inner pointer
    /// </summary>
    /// <returns>Inner pointer</returns>
    inline ptr_type get_inner() const { return m_ptr; }

    /// <summary>
    /// Compares the inner pointer with the given address
    /// </summary>
    /// <param name=in>Address that will be compared</param>
    /// <returns>True if the addresses match</returns>
    template <typename t = address_base_t<ptr_type>>
    inline bool compare(t in) const { return m_ptr == ptr_type(in); }

    /// Actions performed on self

    /// <summary>
    /// Deref inner pointer
    /// </summary>
    /// <param name="in">Times the pointer will be deref'd</param>
    /// <returns>Current address object</returns>
    inline address_base_t<ptr_type> &self_get(uint8_t in = 1)
    {
        m_ptr = get<ptr_type>(in);

        return *this;
    }

    /// <summary>
    /// Add offset to inner pointer
    /// </summary>
    /// <param name="in">Offset that will be added</param>
    /// <returns>Current address object</returns>
    inline address_base_t<ptr_type> &self_offset(ptrdiff_t offset)
    {
        m_ptr += offset;

        return *this;
    }

    /// <summary>
    /// Follows a relative JMP instruction
    /// </summary>
    /// <param name="offset">Offset at which the function address is</param>
    /// <returns>Address object</returns>
    template <typename t = address_base_t<ptr_type>>
    inline address_base_t<ptr_type> &self_jmp(ptrdiff_t offset = 0x1)
    {
        m_ptr = jmp(offset);

        return *this;
    }

    /// <summary>
    /// Finds a specific opcode
    /// </summary>
    /// <param name="opcode">Opcode that is being searched for</param>
    /// <param name="offset">Offset that should be added to the resulting address</param>
    /// <returns>Address object</returns>
    inline address_base_t<ptr_type> &self_find_opcode(std::byte opcode, ptrdiff_t offset = 0x0)
    {
        m_ptr = find_opcode(opcode, offset);

        return *this;
    }

    /// <summary>
    /// Finds a specific opcode sequence
    /// </summary>
    /// <param name="opcodes">Opcodes to be searched</param>
    /// <param name="offset">Offset that should be added to the resulting address</param>
    /// <returns>Address object</returns>
    inline address_base_t<ptr_type> &self_find_opcode_seq(std::vector<std::byte> opcodes, ptrdiff_t offset = 0x0)
    {
        m_ptr = find_opcode_seq(opcodes, offset);

        return *this;
    }

    /// <summary>
    /// Set inner pointer to given value
    /// </summary>
    /// <param name="in">Offset that will be added</param>
    /// <returns>Current address object</returns>
    template <typename t = address_base_t<ptr_type>>
    inline address_base_t<ptr_type> &set(t in)
    {
        m_ptr = ptr_type(in);

        return *this;
    }

    /// Const actions

    /// <summary>
    /// Returns a casted version of the inner pointer
    /// </summary>
    /// <returns>Current address object</returns>
    template <typename t = ptr_type>
    inline t cast() { return t(m_ptr); }

    /// <summary>
    /// Returns a reinterpret_cast version of the inner pointer
    /// </summary>
    /// <returns>Current address object</returns>
    template <typename t = ptr_type>
    inline t as() const { return reinterpret_cast<t>(m_ptr); }

    /// <summary>
    /// Deref inner pointer
    /// </summary>
    /// <param name="in">Times the pointer will be deref'd</param>
    /// <returns>Current address object</returns>
    template <typename t = address_base_t<ptr_type>>
    inline t get(uint8_t in = 1)
    {
        ptr_type dummy = m_ptr;

        while (in--)
            /// Check if pointer is still valid
            if (dummy) dummy = *reinterpret_cast<ptr_type *>(dummy);

        return t(dummy);
    }

    /// <summary>
    /// Add offset to inner pointer
    /// </summary>
    /// <param name="in">Offset that will be added</param>
    /// <returns>Address object</returns>
    template <typename t = address_base_t<ptr_type>>
    inline t offset(ptrdiff_t offset) { return t(m_ptr + offset); }

    /// <summary>
    /// Follows a relative JMP instruction
    /// </summary>
    /// <param name="offset">Offset at which the function address is</param>
    /// <returns>Address object</returns>
    template <typename t = address_base_t<ptr_type>>
    inline t jmp(ptrdiff_t offset = 0x1)
    {
        /// Example:
        /// E9 ? ? ? ?
        /// The offset has to skip the E9 (JMP) instruction
        /// Then deref the address coming after that to get to the function
        /// Since the relative JMP is based on the next instruction after the address it has to be skipped

        /// Base address is the address that follows JMP ( 0xE9 ) instruction
        ptr_type base = m_ptr + offset;

        /// Store the displacement
        /// Note: Displacement addresses can be signed, thanks d3x
        auto displacement = *reinterpret_cast<int32_t *>(base);

        /// The JMP is based on the instruction after the address
        /// so the address size has to be added
        /// Note: This is always 4 bytes, regardless of architecture, thanks d3x
        base += sizeof(uint32_t);

        /// Now finally do the JMP by adding the function address
        base += displacement;

        return t(base);
    }

    /// <summary>
    /// Finds a specific opcode
    /// </summary>
    /// <param name="opcode">Opcode to be searched</param>
    /// <param name="offset">Offset that should be added to the resulting address</param>
    /// <returns>Address object</returns>
    template <typename t = address_base_t<ptr_type>>
    inline t find_opcode(std::byte opcode, ptrdiff_t offset = 0x0)
    {
        auto base = m_ptr;

        auto opcode_at_address = std::byte();

        /// Continue looping as long as address is valid
        while (opcode_at_address = *reinterpret_cast<std::byte *>(base))
        {
            /// Check if we found the opcode we need
            if (opcode == opcode_at_address) break;

            /// Continue searching
            base += 1;
        }

        /// Add additional offset
        base += offset;

        return t(base);
    }

    /// <summary>
    /// Finds a specific opcode sequence
    /// </summary>
    /// <param name="opcode">Opcodes to be searched</param>
    /// <param name="offset">Offset that should be added to the resulting address</param>
    /// <returns>Address object</returns>
    template <typename t = address_base_t<ptr_type>>
    inline t find_opcode_seq(std::vector<std::byte> opcodes, ptrdiff_t offset = 0x0)
    {
        auto base = m_ptr;

        auto opcode_at_address = std::byte();

        /// Continue looping as long as address is valid
        while (opcode_at_address = *reinterpret_cast<std::byte *>(base))
        {
            /// Check if we found the opcode begin
            if (opcodes.at(0) == opcode_at_address)
            {
                /// Check if all following opcodes match too
                for (auto i = 0u; i < opcodes.size(); i++)
                    if (opcodes.at(i) != *reinterpret_cast<std::byte *>(base +
                        i))
                        goto CONT;

                /// We found it!
                break;
            }
        CONT:
            /// Continue searching
            base += 1;
        }

        /// Add additional offset
        base += offset;

        return t(base);
    }
};

/// Adjusted size to architecture
using address_t = address_base_t<uintptr_t>;

/// 32 bit
using address_32_t = address_base_t<uint32_t>;

/// 64 bit
using address_64_t = address_base_t<uint64_t>;

#endif
