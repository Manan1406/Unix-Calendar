/**
 * CSE 29: Systems Programming and Software Tools
 * Spring Quarter 2025
 * Programming Assignment 2
 *
 * calendar.c
 * @file This file contains the implementation of functions for
 * the calendar program that students implement.
 *
 * Author: CSE 29 Spring 2025 PA Team
 * April 2025
 */

/**
 * These are the only imports you are allowed to use.
 * Do not include any other libraries or headers.
 */

#define _GNU_SOURCE // So that you can use strcasestr
#include "calendar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Refer to calendar.h for the required behaviors of each function
static int next_id = 0;
int add_event(week_t *week, time_t start_time, time_t end_time, char *name) {
    // TODO
    int count = 0;
    for (int i = 0; i < 7; i++) {
        if (same_date(start_time, week->days[i].date) == 1) {
            break;
        } else {
            count += 1;
        }
    }
    if(count == 7){
        return -1;
    }

    event_t *head = week->days[count].events;
    while (head != NULL) {
        if (head->end_time > start_time && head->start_time < end_time) {
            return -1;
        } else {
            head = head->next;
        }
    }
    event_t *newEvent = malloc(1 * sizeof(event_t));
    newEvent->id = ++next_id;
    newEvent->name = strdup(name);
    newEvent->start_time = start_time;
    newEvent->end_time = end_time;
    newEvent->next = NULL;
    event_t **helper = &(week->days[count].events);
    while (*helper != NULL && (*helper)->start_time < newEvent->start_time) {
        helper = &((*helper)->next);
    }
    newEvent->next = *helper;
    *helper = newEvent;
    week->days[count].num_events++;

    return newEvent->id;
}

int remove_event(week_t *week, int id) {
    // TODO
    for (int i = 0; i < 7; i++) {
        event_t **helper = &(week->days[i].events);
        while (*helper != NULL) {
            if ((*helper)->id == id) {
                event_t *toRemove = *helper;
                *helper = (*helper)->next;
                free(toRemove->name);
                free(toRemove);
                week->days[i].num_events--;
                return 0;
            }
            helper = &((*helper)->next);
        }
    }
    return -1;
}

int search_event(week_t *week, const char *query, event_t *results[]) {
    // TODO
    int count = 0;
    for (int i = 0; i < 7; i++) {
        event_t *eventFind = week->days[i].events;
        while (eventFind != NULL && count < 10) {
            if (strcasestr(eventFind->name, query) != NULL) {
                results[count] = eventFind;
                count++;
            }
            eventFind = eventFind->next;
        }
    }
    return count;
}

int reschedule_event(week_t *week, int id, time_t start_time, time_t end_time) {
    // TODO
    int flag = -1;
    event_t *event = NULL;
    for (int i = 0; i < 7; i++) {
        event_t **helper = &(week->days[i].events);
        while (*helper != NULL) {
            if ((*helper)->id == id) {
                event = *helper;
                *helper = event->next;
                event->next = NULL;
                flag = 1;
                week->days[i].num_events -= 1;
                break;
            }
            helper = &((*helper)->next);
        }
        if (flag == 1) {
            break;
        }
    }
    if (flag == -1) {
        return 1;
    }
    char *name = strdup(event->name);
    int temp_next_id = next_id;
    next_id = event->id - 1;
    int add_result = add_event(week, start_time, end_time, name);
    if (add_result == -1) {
        add_event(week, event->start_time, event->end_time, name);
        next_id = temp_next_id;
        free(event->name);
        free(event);
        free(name);
        return -1;
    }
    free(name);
    next_id = temp_next_id;
    free(event->name);
    free(event);
    return 0;
}

int duplicate_event(week_t *week, int id, int day) {
    // TODO
    event_t *event = NULL;
    int flag = -1;
    for (int i = 0; i < 7; i++) {
        event_t **helper = &(week->days[i].events);
        while (*helper != NULL) {
            if ((*helper)->id == id) {
                event = *helper;
                flag = 1;
            }
            helper = &((*helper)->next);
        }
        if (flag == 1) {
            break;
        }
    }
    if (flag == -1) {
        return -2;
    }
    if (same_date(event->start_time, week->days[day].date)) {
        return -1;
    }
    time_t new_start_time =
        combine_date_time(week->days[day].date, event->start_time);
    time_t new_end_time =
        combine_date_time(week->days[day].date, event->end_time);
    return add_event(week, new_start_time, new_end_time, event->name);
}

void free_week(week_t *week) {
    // TODO
    for (int i = 0; i < 7; i++) {
        event_t *curr = week->days[i].events;
        while (curr != NULL) {
            event_t *next = curr->next;
            free(curr->name);
            free(curr);
            curr = next;
        }
    }
    free(week);
}
