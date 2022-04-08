// Header used to include the unity unit test framework library
#include "unity.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#undef NDEBUG

#include <vlc_common.h>
#include "vlc_arrays.h"

#include <stdio.h>
#include <stdlib.h>

static void test_dictionary_validity (vlc_dictionary_t * p_dict, const char ** our_keys, int size )
{
    /* Test values and keys now */
    char ** keys = vlc_dictionary_all_keys( p_dict );
    intptr_t i, j;

    TEST_ASSERT( keys );     // Checks if a valid key was entered

    for( j = 0; keys[j]; j++ )
    {
        bool found = false;
        for( i = 0; i < size; i++ )
        {
            if(!strcmp( keys[j], our_keys[i] ))     // Checks to see if the key entered matches a valid key
            {
                found = true;
                break;
            }
        }
        free( keys[j] );
        TEST_ASSERT( found );     // Checks to see if the key is found
    }
    free( keys );

    for( i = 0; i < size; i++ )
    // Checks to see if dictionary value of key equals the corresponding size
        TEST_ASSERT( vlc_dictionary_value_for_key( p_dict, our_keys[i] ) == (void *)i );
}

int main (void)
{
    static const char * our_keys[] = {
        "Hello", "Hella", "flowmeter", "Frostnipped", "frostnipped", "remiform", "quadrifoliolate", "singularity", "unafflicted"
    };
    const int size = sizeof(our_keys)/sizeof(our_keys[0]);
    char ** keys;
    intptr_t i = 0;

    vlc_dictionary_t dict;
    vlc_dictionary_init( &dict, 0 );

    TEST_ASSERT( vlc_dictionary_keys_count( &dict ) == 0 );     // Checks whether or not dictionary key is 0
    TEST_ASSERT( vlc_dictionary_is_empty( &dict ) );     // Checks whether or not dictionary is empty

    keys = vlc_dictionary_all_keys( &dict );
    TEST_ASSERT( keys && !keys[0] );     // Checks for any duplicate keys
    free(keys);


    /* Insert some values */
    for( i = 0; i < size; i++ )
    {
        vlc_dictionary_insert( &dict, our_keys[i], (void *)i );
        TEST_ASSERT( vlc_dictionary_has_key(&dict, our_keys[i]) );     // Checks if key is inserted
        for( int j = i + 1; j < size; j++ )
            TEST_ASSERT( !vlc_dictionary_has_key(&dict, our_keys[j]) );     // If key is not inserted
    }

    TEST_ASSERT( !vlc_dictionary_is_empty( &dict ) );     // Checks whether or not the dictionary is empty
    test_dictionary_validity( &dict, our_keys, size );

    vlc_dictionary_remove_value_for_key( &dict, our_keys[size-1], NULL, NULL );

    RUN_TEST(test_dictionary_validity( &dict, our_keys, size-1 ));     // Runs function test for validity

    vlc_dictionary_clear( &dict, NULL, NULL );

    TEST_ASSERT( vlc_dictionary_keys_count( &dict ) == 0 );     // Checks at end if the dictionary key is 0
    TEST_ASSERT( vlc_dictionary_is_empty( &dict ) );     // Checks at end if dictionary is empty
    return 0;
}
