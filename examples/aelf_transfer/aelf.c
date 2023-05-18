#include <stdio.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "aelf.pb.h"
#include "libbase58.h"

#define BUFFER_SIZE 256

typedef struct _BufferHolder
{
    pb_byte_t *buffer;
    /* The amount to to transfer. */
    size_t size;
} BufferHolder;

void binaryToHex(char* binary, char* hex, int len) {
    char* hexMap = "0123456789ABCDEF";
    int i = 0;

    // Process every 4 bits
    while(i < len) {
        // Get the binary digit by subtracting '0'
        int byte = binary[i] - '0';

        // Bitwise shift to the left by three places
        byte = byte << 3;

        // Take the next three bits and bitwise or them with the first bit
        byte = byte | ((binary[i + 1] - '0') << 2);
        byte = byte | ((binary[i + 2] - '0') << 1);
        byte = byte | (binary[i + 3] - '0');

        // Get the hex representation from the map
        hex[i / 4] = hexMap[byte];
        
        i += 4;
    }
    hex[len / 4] = '\0';
}

void tohex(unsigned char * in, size_t insz, char * out, size_t outsz)
{
    unsigned char * pin = in;
    const char * hex = "0123456789ABCDEF";
    char * pout = out;
    for(; pin < in+insz; pout +=2, pin++){
        pout[0] = hex[(*pin>>4) & 0xF];
        pout[1] = hex[ *pin     & 0xF];
        if (pout + 2 - out > outsz){
            /* Better to truncate output string than overflow buffer */
            /* it would be still better to either return a status */
            /* or ensure the target buffer is large enough and it never happen */
            break;
        }
    }
    pout[-1] = 0;
}
// typedef struct _TransferInfo
// {
//     char *to;
//     char *symbol;
//     /* The amount to to transfer. */
//     int64_t amount;
//     /* The memo. */
//     char *memo;
// } TransferInfo;

bool read_address_field(pb_istream_t *stream, const pb_field_iter_t *field, void **arg)
{
    BufferHolder* bufferHolder = *arg;
    pb_byte_t binBuffer[BUFFER_SIZE] = {0};
    size_t binToRead = stream->bytes_left;
    bool status = pb_read(stream, binBuffer, binToRead);
    if (!status)
        return status;
    // binaryToHex((char*) binBuffer, (char*) bufferHolder->buffer, binToRead);
    tohex(binBuffer, binToRead, (char*) bufferHolder->buffer, 72);
    return true;
}

bool read_string_field(pb_istream_t *stream, const pb_field_iter_t *field, void **arg)
{
    BufferHolder* bufferHolder = *arg;
    return pb_read(stream, bufferHolder->buffer, stream->bytes_left);
}

bool read_transfer_input(pb_istream_t *stream, const pb_field_iter_t *field, void **arg)
{
    bool status = pb_decode(stream, aelf_TransferInput_fields, *arg);
    return status;
}

int main()
{
    char txn_data[] = {
        0x0a, 0x22, 0x0a, 0x20, 0xcd, 0xef, 0xe7, 0x28,
        0x49, 0x31, 0x33, 0xf5, 0x26, 0xcb, 0x5e, 0x97,
        0xe2, 0xec, 0x33, 0x9c, 0xa2, 0x19, 0xbe, 0xf8,
        0x04, 0x27, 0xea, 0xf5, 0x3f, 0xf0, 0x00, 0x3c,
        0xad, 0x24, 0x1c, 0x7a, 0x12, 0x22, 0x0a, 0x20,
        0x27, 0x91, 0xe9, 0x92, 0xa5, 0x7f, 0x28, 0xe7,
        0x5a, 0x11, 0xf1, 0x3a, 0xf2, 0xc0, 0xae, 0xc8,
        0xb0, 0xeb, 0x35, 0xd2, 0xf0, 0x48, 0xd4, 0x2e,
        0xba, 0x89, 0x01, 0xc9, 0x2e, 0x03, 0x78, 0xdc,
        0x18, 0x8d, 0xcd, 0xa2, 0x4b, 0x22, 0x04, 0x5a,
        0x74, 0x47, 0x38, 0x2a, 0x08, 0x54, 0x72, 0x61,
        0x6e, 0x73, 0x66, 0x65, 0x72, 0x32, 0x34, 0x0a,
        0x22, 0x0a, 0x20, 0x4f, 0xf4, 0xe6, 0x3a, 0xd4,
        0xaa, 0x7e, 0xc9, 0x2e, 0x65, 0xba, 0x2d, 0x37,
        0xb2, 0xc5, 0x6b, 0x3f, 0x82, 0x39, 0x0b, 0xfc,
        0x25, 0xe6, 0x6c, 0xeb, 0xab, 0x68, 0x21, 0xf3,
        0xb0, 0x5c, 0x0b, 0x12, 0x03, 0x45, 0x4c, 0x46,
        0x18, 0x80, 0xad, 0xe2, 0x04, 0x22, 0x04, 0x74,
        0x65, 0x73, 0x74, 0x82, 0xf1, 0x04, 0x41, 0xf0,
        0x82, 0xe6, 0xda, 0x05, 0x8f, 0x7a, 0x3d, 0xf4,
        0x6a, 0xd6, 0x39, 0xe6, 0xaa, 0x7c, 0xa4, 0xf6,
        0xe3, 0x8c, 0x87, 0xbd, 0xc2, 0xbd, 0x96, 0x23,
        0x4f, 0xe1, 0x98, 0xb5, 0x30, 0xb4, 0xe0, 0x20,
        0xc1, 0xa3, 0xab, 0x04, 0xed, 0x92, 0xc2, 0x05,
        0x71, 0xce, 0x14, 0xa3, 0x37, 0x24, 0xb8, 0xfa,
        0xd3, 0x1d, 0x19, 0xfc, 0xe0, 0xb1, 0xea, 0xaf,
        0x3c, 0xf3, 0xa1, 0x4e, 0x60, 0xdf, 0x7d, 0x01};

    aelf_TransferInput transfer_input = aelf_TransferInput_init_zero;

    pb_byte_t symbolBuffer[BUFFER_SIZE] = {0};
    size_t symbolBufferSize = sizeof(symbolBuffer);
    pb_byte_t memoBuffer[BUFFER_SIZE] = {0};
    size_t memoBufferSize = sizeof(memoBuffer);
    pb_byte_t addressBuffer[BUFFER_SIZE] = {0};
    size_t addressBufferSize = sizeof(addressBuffer);

    transfer_input.symbol.funcs.decode = read_string_field;
    transfer_input.memo.funcs.decode = read_string_field;
    transfer_input.to.value.funcs.decode = read_address_field;
    BufferHolder symbolArg;
    symbolArg.buffer = symbolBuffer;
    symbolArg.size = symbolBufferSize;
    transfer_input.symbol.arg = &symbolArg;
    BufferHolder memoArg;
    memoArg.buffer = memoBuffer;
    memoArg.size = memoBufferSize;
    transfer_input.memo.arg = &memoArg;

    BufferHolder addressArg;
    addressArg.buffer = addressBuffer;
    addressArg.size = addressBufferSize;
    transfer_input.to.value.arg = &addressArg;

    // pb_byte_t toAddress[BUFFER_SIZE];

    // TransferInfo info;
    // info.to = toAddress;
    // info.symbol = symbolBuffer;
    // info.memo = memoBuffer;

    // void* arg[] = {
    //     &transfer_input
    // };

    /* Allocate space for the decoded message. */
    aelf_Transaction txn = aelf_Transaction_init_zero;

    txn.params.funcs.decode = read_transfer_input;
    txn.params.arg = &transfer_input;
    /* Create a stream that reads from the buffer. */
    pb_istream_t stream = pb_istream_from_buffer((const pb_byte_t *)txn_data, sizeof(txn_data));

    // printf("transfer location is %p\n", &transfer_input);

    // printf("arg0 location is %p\n", arg[0]);

    /* Now we are ready to decode the message. */
    bool status;
    status = pb_decode(&stream, aelf_Transaction_fields, &txn);
    /* Check for errors... */
    if (!status)
    {
        printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    printf("Address   : %s\n", addressBuffer);
    printf("Symbol    : %s\n", symbolBuffer);
    printf("Amount    : %llu\n", transfer_input.amount);
    printf("Memo      : %s\n", memoBuffer);

    return 0;
}
