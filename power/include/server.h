/*
 * =====================================================================================
 *
 *       Filename:  server.h
 *
 *    Description:  ZMQ functions for server side
 *
 *        Version:  1.0
 *        Created:  25/12/2022 19:17:39
 *       Revision:  none
 *       Compiler:  clang
 *
 *         Author:  Idriss Daoudi <idaoudi@anl.gov>
 *   Organization:  Argonne National Laboratory
 *
 * =====================================================================================
 */

#include <stdlib.h>

int tpm_zmq_connect_server(void *server);

int tpm_zmq_connect_server(void *server) {
  int num = 0;
  zmq_setsockopt(server, ZMQ_LINGER, &num, sizeof(int));
  num = -1;
  zmq_setsockopt(server, ZMQ_SNDHWM, &num, sizeof(int));
  zmq_setsockopt(server, ZMQ_RCVHWM, &num, sizeof(int));
  int ret = zmq_bind(server, "tcp://127.0.0.1:5555");

  return ret;
}
