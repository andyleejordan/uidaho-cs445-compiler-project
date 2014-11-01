/*
 * logger.h - Interface for logging system.
 *
 * Copyright (C) 2014 Andrew Schwartzmeyer
 *
 * This file released under the AGPLv3.
 *
 */

#ifndef LOGGER_H
#define LOGGER_H

struct tree;

void log_error(const char *format, ...);
void log_debug(const char *format, ...);
void log_crash();

void log_lexical(const char *format, ...);
void log_semantic(struct tree *n, const char *format, ...);
void log_check(const char *format, ...);
void log_unsupported();

#endif /* LOGGER_H */
