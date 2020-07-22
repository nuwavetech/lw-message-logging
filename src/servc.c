/**
 *
 *  Copyright (c) 2020 NuWave Technologies, Inc. All rights reserved.
 *
 */
#pragma nolist

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cextdecs>
#include <tal.h>
#include <zsysc>

#include "lwml.h"

#pragma list

#define TWO_MB 2097152
#define CONTINUE_DIALOG 70
#define ABORT_DIALOG 1

void printNameValuePairs(const char* base, unsigned int offset,
                         unsigned int count);

int main(int argc, char** argv, char** env) {

  short recvFilenum;
  short rc;
  short cc;
  unsigned int done = 0;
  unsigned int openCount = 0;
  char* recvBuffer;
  lw_ml_msg_log_rq_def* request;
  int countRead;
  char* base;
  int streaming;
  size_t streamRead;

  /* Open $RECEIVE. */
  rc = FILE_OPEN_("$RECEIVE", 8, &recvFilenum, /*access*/, /*excl*/, /*depth*/,
                  1);
  if (rc != 0) {
    printf("Open on $RECEIVE failed: %d\n", (int)rc);
    return 0;
  }

  if ((recvBuffer = calloc(1, TWO_MB)) == NULL) {
    printf("Unable to allocate request buffer\n");
    return 0;
  }

  streaming = 0;
  while (!done) {
    cc = READUPDATEXL(recvFilenum, recvBuffer, TWO_MB, &countRead);

    if (_status_lt(cc)) /* file system error */
    {
      printf("READUPATE $RECEIVE I/O error %d\n");
      break;
    }

    if (_status_gt(cc)) /* system msg */
    {
      short* code = (short*)recvBuffer;

      switch (*code) {
      case ZSYS_VAL_SMSG_OPEN:
        openCount++;
        break;
      case ZSYS_VAL_SMSG_CLOSE:
        openCount--;

        if (openCount == 0) {
          done = 1;
        }
        break;
      default:
        break;
      }

      REPLYXL();
      continue;
    }

    /* Read the request. It may arrive in a single I/O completion or as
    a stream in sequential I/O completions withing a Pathway dialog. */
    if (!streaming) {
      request = (lw_ml_msg_log_rq_def*)recvBuffer;
      /* If the request is larger than the countRead, then setup the request
      for streaming. Allocate a buffer sufficient for the entire request,
      copy what we've received into it, and reply with the continue dialog
      error-return code (70). */
      if (request->header.rq_len > countRead) {
        streaming = 1;
        request = malloc(request->header.rq_len);
        memcpy(request, recvBuffer, countRead);
        streamRead = countRead;
        printf("SERVC log message streaming:\n");
        printf("      read %10d %10d of %d received.\n", countRead, streamRead,
               request->header.rq_len);
        REPLYXL(/*buffer*/, /*write-count*/, /*count-written*/, /*message-tag*/,
                CONTINUE_DIALOG);
        continue;
      } else {
        /* The request is in a single recv. */
        printf("SERVC log message received:   %d\n", countRead);
        REPLYXL();
      }
    } else {
      /* We've received the next buffer in the stream. Check to make sure
      we haven't received too much (this should not happen). If so, abort the
      dialog. */
      if ((countRead + streamRead) > request->header.rq_len) {
        printf("SERVC stream processing error, too much data received\n");
        streaming = 0;
        REPLYXL(/*buffer*/, /*write-count*/, /*count-written*/, /*message-tag*/,
                ABORT_DIALOG);
        continue;
      } else {
        /* Copy the buffer we received to the request buffer at the next offset.
         */
        memcpy(&((char*)request)[streamRead], recvBuffer, countRead);
        streamRead += countRead;
        printf("      read %10d %10d of %d received.\n", countRead, streamRead,
               request->header.rq_len);
        /* If we haven't received all of the data yet, continue the dialog. If
        we have, end the dialog by replying with error-return code 0. */
        if (streamRead < request->header.rq_len) {
          REPLYXL(/*buffer*/, /*write-count*/, /*count-written*/,
                  /*message-tag*/, CONTINUE_DIALOG);
          continue;
        } else {
          printf("\n");
          REPLYXL();
          streaming = 0;
        }
      }
    }

    /* Dump the request. */
    base = (char*)request;

    printf("  code                    %hd\n", request->header.rq_code);
    printf("  version                 %hd\n", request->header.rq_version);
    printf("  len                     %u\n", request->header.rq_len);
    printf("  id                      %lld\n", request->header.rq_ts_unique[0]);
    printf("                          %lld\n", request->header.rq_ts_unique[1]);

    printf("  rq-start-time           %lld\n", request->map.start_time);
    printf("  rq-end-time             %lld\n", request->map.end_time);
    printf("  rq-total-time           %lld\n", request->map.total_time);
    printf("  rq-connect-time         %lld\n", request->map.connect_time);
    printf("  rq-connect-hs-time      %lld\n", request->map.connect_hs_time);
    printf("  rq-request-time         %lld\n", request->map.request_time);
    printf("  rq-response_time        %lld\n", request->map.response_time);
    printf("  rq-serialize-time       %lld\n", request->map.serialize_time);
    printf("  rq-deserialize-time     %lld\n", request->map.deserialize_time);
    printf("  rq-server-io            %lld\n", request->map.server_io_time);

    printf("  rq-ipm-offset           %u\n", request->map.rq_ipm_offset);
    printf("  rq-ipm-len              %u\n", request->map.rq_ipm_len);

    printf("  rq-request-line-offset  %u\n",
           request->map.rq_request_line_offset);
    printf("  rq-request-line-len     %u\n", request->map.rq_request_line_len);
    printf("  rq-request-line         %s\n",
           &base[request->map.rq_request_line_offset]);
  
    printf("  rq-method-off           %u\n", request->map.rq_method_offset);
    printf("  rq-method-len           %u\n", request->map.rq_method_len);
    printf("  rq-method               %s\n",
           &base[request->map.rq_method_offset]);

    printf("  rq-uri-off              %u\n", request->map.rq_uri_offset);
    printf("  rq-uri-len              %u\n", request->map.rq_uri_len);
    printf("  rq-uri                  %s\n", &base[request->map.rq_uri_offset]);

    printf("  rq-params-offset        %u\n", request->map.rq_params_offset);
    printf("  rq-params-len           %u\n", request->map.rq_params_len);
    printf("  rq-params-count         %u\n", request->map.rq_params_count);
    printNameValuePairs(base, request->map.rq_params_offset,
                        request->map.rq_params_count);

    printf("  rq-request-http-ver-off %u\n", request->map.rq_http_ver_offset);
    printf("  rq-request-http-ver-len %u\n", request->map.rq_http_ver_len);
    printf("  rq-request-http-ver     %s\n",
           &base[request->map.rq_http_ver_offset]);

    printf("  rq-headers-offset       %u\n", request->map.rq_headers_offset);
    printf("  rq-headers-len          %u\n", request->map.rq_headers_len);
    printf("  rq-headers-count        %u\n", request->map.rq_headers_count);
    printNameValuePairs(base, request->map.rq_headers_offset,
                        request->map.rq_headers_count);

    printf("  rq-body-offset          %u\n", request->map.rq_body_offset);
    printf("  rq-body-len             %u\n", request->map.rq_body_len);

    printf("  rp-ipm-offset           %u\n", request->map.rp_ipm_offset);
    printf("  rp-ipm-len              %u\n", request->map.rp_ipm_len);

    printf("  rp-status               %u\n", request->map.rp_status);
    printf("  rp-status-line-offset   %u\n",
           request->map.rp_status_line_offset);
    printf("  rp-status-line-len      %u\n", request->map.rp_status_line_len);
    printf("  rp-status-line          %s\n",
           &base[request->map.rp_status_line_offset]);

    printf("  rp-headers-offset       %u\n", request->map.rp_headers_offset);
    printf("  rp-headers-len          %u\n", request->map.rp_headers_len);
    printf("  rp-headers-count        %u\n", request->map.rp_headers_count);
    printNameValuePairs(base, request->map.rp_headers_offset,
                        request->map.rp_headers_count);

    printf("  rp-body-offset          %u\n", request->map.rp_body_offset);
    printf("  rp-body-len             %u\n", request->map.rp_body_len);

    printf("  user-data-offset        %u\n", request->map.user_data_offset);
    printf("  user-data-len           %u\n", request->map.user_data_len);
    printf("  user-data-count         %u\n", request->map.user_data_count);
    printNameValuePairs(base, request->map.user_data_offset,
                        request->map.user_data_count);

    printf("  metadata-offset         %u\n", request->map.metadata_offset);
    printf("  metadata-len            %u\n", request->map.metadata_len);
    printf("  metadata-count          %u\n", request->map.metadata_count);
    printNameValuePairs(base, request->map.metadata_offset,
                        request->map.metadata_count);

    printf("end of request\n\n");

    /* If the request buffer is not the recvBuffer, then it was allocated
    for a streamed request, so free it. */
    if ((char*)request != recvBuffer) {
      free(request);
    }
  }

  FILE_CLOSE_(recvFilenum);
  free(recvBuffer);
  return 0;
}

void printNameValuePairs(const char* base, unsigned int offset,
                         unsigned int count) {
  const char* p;
  int i;

  p = base + offset;
  for (i = 0; i < count; i++) {
    printf("        %s : ", p);
    p += strlen(p) + 1;
    printf("%s\n", p);
    p += strlen(p) + 1;
  }
}
