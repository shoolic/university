#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"

extern void scale_image(void *src, int sw, int sh, void *dst, int dw, int dh);

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        exit(1);
    }

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *queue = NULL;
    ALLEGRO_BITMAP *source = NULL;
    ALLEGRO_BITMAP *scaled = NULL;
    ALLEGRO_LOCKED_REGION *src_pixels = NULL;
    ALLEGRO_LOCKED_REGION *scaled_pixels = NULL;

    al_init();
    al_init_image_addon();
    al_install_keyboard();

    al_set_new_display_flags(ALLEGRO_WINDOWED);
    display = al_create_display(200, 200);
    queue = al_create_event_queue();

    al_set_new_bitmap_flags(ALLEGRO_PIXEL_FORMAT_ANY_32_NO_ALPHA);
    source = al_load_bitmap_flags(argv[1], ALLEGRO_PIXEL_FORMAT_ANY_32_NO_ALPHA);
    src_pixels = al_lock_bitmap(source, 0, ALLEGRO_LOCK_READONLY);

    const int src_height = al_get_bitmap_height(source);
    const int src_width = al_get_bitmap_width(source);

    int height = src_height;
    int width = src_width;

    ALLEGRO_EVENT event;
    ALLEGRO_KEYBOARD_STATE key_state;
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());

    int run = 1;
    int change_value = 1;
    bool size_changed = true;

    while (run)
    {
        if (!al_is_event_queue_empty(queue))
        {
            al_wait_for_event(queue, &event);
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                run = false;
            }
        }

        al_get_keyboard_state(&key_state);

        if (al_key_down(&key_state, ALLEGRO_KEY_1))
        {
            change_value = 1;
        }

        if (al_key_down(&key_state, ALLEGRO_KEY_2))
        {
            change_value = 8;
        }

        if (al_key_down(&key_state, ALLEGRO_KEY_3))
        {
            change_value = 16;
        }

        if (al_key_down(&key_state, ALLEGRO_KEY_UP))
        {
            height += change_value;
            size_changed = true;
        }

        if (al_key_down(&key_state, ALLEGRO_KEY_DOWN))
        {
            if (height > change_value)
            {
                height -= change_value;
                size_changed = true;
            }
        }

        if (al_key_down(&key_state, ALLEGRO_KEY_RIGHT))
        {
            width += change_value;
            size_changed = true;
        }

        if (al_key_down(&key_state, ALLEGRO_KEY_LEFT))
        {
            if (width > change_value)
            {
                width -= change_value;
                size_changed = true;
            }
        }

        if (size_changed)
        {
            al_resize_display(display, width, height);
            al_destroy_bitmap(scaled);

            scaled = al_create_bitmap(width, height);
            scaled_pixels = al_lock_bitmap(scaled, 0, ALLEGRO_LOCK_READWRITE);

            scale_image(src_pixels->data, src_width, src_height, scaled_pixels->data, width, height);

            al_draw_bitmap(scaled, 0, 0, 0);
            al_flip_display();
            size_changed = false;
        }
    }

    al_destroy_display(display);
    al_uninstall_keyboard();
    al_destroy_bitmap(scaled);
    al_destroy_bitmap(source);

    return 0;
}
