#pragma once

#define NO_COPY(type)           \
    type(const type&) = delete; \
    type& operator=(const type&) = delete;
