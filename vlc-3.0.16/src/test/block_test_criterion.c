// Header file necessary for criterion unit test
#include <criterion/criterion.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#undef NDEBUG

#include <vlc_common.h>
#include <vlc_block.h>

static const char text[] =
    "This is a test!\n"
    "This file can be deleted safely!\n";

// Function to test each block file depending on write mode
static void test_block_File(bool write)
{
    FILE *stream;
    int res;

    // Open test file using read and write binary permissions
    stream = fopen ("testfile.txt", "wb+e");
    cr_assert (stream != NULL);     // Check if the file opened

    res = fputs (text, stream);     // Write text to file
    cr_assert (res != EOF);     // Check if the text reaches the end of file
    res = fflush (stream);     // Remove all of the text in file
    cr_assert (res != EOF);     // Check if the text reaches the end of file after removing text

    block_t *block = block_File(fileno(stream), write);     // Declare and initialize a block type for the file
    fclose (stream);     // Close file

    cr_assert (block != NULL);     // Check whether or not the block is empty
    cr_assert (block->i_buffer == strlen (text));     // Check if the blocks buffer is equal to the length of the text
    cr_assert (!memcmp (block->p_buffer, text, block->i_buffer));   // Check if the memory of both buffers is not equal
    // If in write mode, set the specific buffer value
    if (write)
        memset(block->p_buffer, 'A', block->i_buffer);
    block_Release (block);

    remove ("testfile.txt");
}

// Function to test each block
static void test_block (void)
{
    block_t *block = block_Alloc (sizeof (text));
    cr_assert (block != NULL);

    memcpy (block->p_buffer, text, sizeof (text));
    block = block_Realloc (block, 0, sizeof (text));
    cr_assert (block != NULL);
    cr_assert (block->i_buffer == sizeof (text));
    cr_assert (!memcmp (block->p_buffer, text, sizeof (text)));

    // Set the value of block equal to specific text location
    block = block_Realloc (block, 200, sizeof (text) + 200);
    cr_assert (block != NULL);     // Check whether or not the block is empty
    cr_assert (block->i_buffer == 200 + sizeof (text) + 200);     // Check if the buffer is equal to the text location
    cr_assert (!memcmp (block->p_buffer + 200, text, sizeof (text)));

    block = block_Realloc (block, -200, sizeof (text) + 200);     // Move block text location
    cr_assert (block != NULL);
    cr_assert (block->i_buffer == sizeof (text));
    cr_assert (!memcmp (block->p_buffer, text, sizeof (text)));
    block_Release (block);

}

int main (void)
{
  // Call test functions to run block file with write mode of true and false and test block function
    test_block_File(false);
    test_block_File(true);
    test_block ();
    return 0;
}
