// Header used to include the check unit test framework library
#include <check_stdint.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#undef NDEBUG

#include <vlc_common.h>
#undef msleep

// Timer data structure
struct timer_data
{
    vlc_timer_t timer;
    vlc_mutex_t lock;
    vlc_cond_t  wait;
    unsigned count;
};

static void callback (void *ptr)
{
    struct timer_data *data = ptr;

    vlc_mutex_lock (&data->lock);
    data->count += 1 + vlc_timer_getoverrun (data->timer);
    vlc_cond_signal (&data->wait);
    vlc_mutex_unlock (&data->lock);
}


int main (void)
{
    struct timer_data data;
    mtime_t ts;
    int val;

    vlc_mutex_init (&data.lock);
    vlc_cond_init (&data.wait);
    data.count = 0;

    // Checks if the value of timer is 0
    val = vlc_timer_create (&data.timer, callback, &data);
    ck_assert (val == 0);
    vlc_timer_destroy (data.timer);
    // Checks if the data count is equal to 0
    ck_assert (data.count == 0);

    val = vlc_timer_create (&data.timer, callback, &data);
    ck_assert (val == 0);
    vlc_timer_schedule (data.timer, false, CLOCK_FREQ << 20, CLOCK_FREQ);     // Schedules and sets the timer
    vlc_timer_destroy (data.timer);     // Destroys the timer
    ck_assert (data.count == 0);

    val = vlc_timer_create (&data.timer, callback, &data);
    ck_assert (val == 0);

    /* Relative timer */
    ts = mdate ();
    vlc_timer_schedule (data.timer, false, 1, CLOCK_FREQ / 100);

    vlc_mutex_lock (&data.lock);
    while (data.count <= 10)
        vlc_cond_wait(&data.wait, &data.lock);

    ts = mdate () - ts;
    printf ("%u iterations in %"PRId64" us\n", data.count, ts);
    data.count = 0;
    vlc_mutex_unlock (&data.lock);
    ck_assert(ts >= (CLOCK_FREQ / 10));     // Checks if the time in seconds is greater than or equal to clock freq

    vlc_timer_schedule (data.timer, false, 0, 0);

    /* Absolute timer */
    ts = mdate ();

    vlc_timer_schedule (data.timer, true, ts + CLOCK_FREQ / 10,
                        CLOCK_FREQ / 100);

    vlc_mutex_lock (&data.lock);
    while (data.count <= 10)
        vlc_cond_wait(&data.wait, &data.lock);

    ts = mdate () - ts;
    printf ("%u iterations in %"PRId64" us\n", data.count, ts);
    vlc_mutex_unlock (&data.lock);
    ck_assert(ts >= (CLOCK_FREQ / 5));

    vlc_timer_destroy (data.timer);
    vlc_cond_destroy (&data.wait);
    vlc_mutex_destroy (&data.lock);

    return 0;
}
