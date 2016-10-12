---
title: "struct clubby_frame_info"
decl_name: "struct clubby_frame_info"
symbol_kind: "struct"
signature: |
  struct clubby_frame_info {
    const char *channel_type; /* Type of the channel this message arrived on. */
    bool channel_is_trusted;  /* Whether the channel is marked as trusted. */
  };
---

Auxiliary information about the request or response. 

