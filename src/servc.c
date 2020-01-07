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

    /* Dump message. */
    request = (lw_ml_msg_log_rq_def*)recvBuffer;
    base = recvBuffer;

    printf("SERVC $RECEIVE message:   %d\n", countRead);

    printf("  code                    %hd\n", request->header.rq_code);
    printf("  version                 %hd\n", request->header.rq_version);
    printf("  len                     %u\n", request->header.rq_len);
    printf("  id                      %lld\n", request->header.rq_ts_unique[0]);
    printf("                          %lld\n", request->header.rq_ts_unique[1]);

    printf("  rq-start-time           %lld\n", request->map.start_time);
    printf("  rq-end-time             %lld\n", request->map.end_time);

    printf("  rq-ipm-offset           %u\n", request->map.rq_ipm_offset);
    printf("  rq-ipm-len              %u\n", request->map.rq_ipm_len);

    printf("  rq-request-line-offset  %u\n",
           request->map.rq_request_line_offset);
    printf("  rq-request-line-len     %u\n", request->map.rq_request_line_len);
    printf("  rq-request-line         %s\n",
           &base[request->map.rq_request_line_offset]);

    printf("  rq-headers-offset       %u\n", request->map.rq_headers_offset);
    printf("  rq-headers-len          %u\n", request->map.rq_headers_len);
    printf("  rq-headers-count        %u\n", request->map.rq_headers_count);
    printNameValuePairs(base, request->map.rq_headers_offset,
                        request->map.rq_headers_count);

    printf("  rq-payload-offset       %u\n", request->map.rq_payload_offset);
    printf("  rq-payload-len          %u\n", request->map.rq_payload_len);

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

    printf("  rp-payload-offset       %u\n", request->map.rp_payload_offset);
    printf("  rp-payload-len          %u\n", request->map.rp_payload_len);

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
