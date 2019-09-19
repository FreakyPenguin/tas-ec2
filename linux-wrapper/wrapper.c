/*
 * Copyright 2019 University of Washington, Max Planck Institute for
 * Software Systems, and The University of Texas at Austin
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <inttypes.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

#include <netlink/netlink.h>
#include <netlink/route/link.h>
#include <netlink/route/addr.h>
#include <systemd/sd-daemon.h>

#include <tas_ll_connect.h>

static int interface_mac_set(const char *name, uint64_t mac);

static int interface_mac_set(const char *name, uint64_t mac)
{
  struct nl_sock *sock;
  struct nl_cache *cache_link;
  struct rtnl_link *link, *newlink;
  struct nl_addr *na;
  int err, ret = 0;

  if ((sock = nl_socket_alloc()) == NULL) {
    perror("nl_socket_alloc");
    return -1;
  }

  if ((err = nl_connect(sock, NETLINK_ROUTE)) < 0) {
    nl_perror(err, "nl_connect");
    ret = -1;
    goto exit_socket_free;
  }

  if ((err = rtnl_link_alloc_cache(sock, AF_UNSPEC, &cache_link)) < 0) {
    nl_perror(err, "rtnl_link_alloc_cache");
    ret = -1;
    goto exit_socket_close;
  }

  if ((link = rtnl_link_get_by_name(cache_link, name)) == NULL) {
    nl_perror(err, "rtnl_link_get_by_name");
    ret = -1;
    goto exit_cache_free;
  }

  if ((newlink = rtnl_link_alloc()) == NULL) {
    nl_perror(err, "rtnl_link_alloc");
    ret = -1;
    goto exit_cache_free;
  }

  if ((na = nl_addr_alloc(6)) == NULL) {
    nl_perror(err, "nl_addr_alloc");
    ret = -1;
    goto exit_cache_free;
  }

  nl_addr_set_family(na, AF_LLC);
  nl_addr_set_binary_addr(na, &mac, 6);
  nl_addr_set_prefixlen(na, 48);

  rtnl_link_set_addr(newlink, na);
  if ((err = rtnl_link_change(sock, link, newlink, 0)) != 0) {
    nl_perror(err, "rtnl_link_change");
    ret = -1;
    goto exit_cache_free;
  }

exit_cache_free:
  nl_cache_free(cache_link);
exit_socket_close:
  nl_close(sock);
exit_socket_free:
  nl_socket_free(sock);
  return ret;
}

static int start_tas(int argc, char *argv[])
{
  pid_t pid;

  pid = fork();
  if (pid == 0) {
    // in child
    execvp("/home/ubuntu/tas/code/tas/tas", argv);
    exit(1);
  } else if (pid < 0) {
    return -1;
  }
  return 0;
}

static char *ifname_find(int argc, char *argv[])
{
  const char *pref = "--kni-name=";
  size_t pref_len = strlen(pref);
  int i;

  for (i = 1; i < argc; i++) {
    if (strncmp(pref, argv[i], pref_len) == 0) {
      return argv[i] + pref_len;
    }
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  struct flexnic_info *info;
  void *mem;
  char *ifname;
  pid_t pid;
  int status;

  if (start_tas(argc, argv))
    return 1;

  sleep(2);

  if (flexnic_driver_connect(&info, &mem) != 0) {
    perror("flexnic_driver_connect");
    return 1;
  }

  while ((info->flags & FLEXNIC_FLAG_READY) == 0) {
    usleep(100000);
  }
  printf("tas ready: %lx\n", info->mac_address);

  if ((ifname = ifname_find(argc, argv)) != NULL) {
    printf("kni if: %s\n", ifname);
    if (interface_mac_set(ifname, info->mac_address) != 0) {
      perror("macset failed");
    }
  }

  if (sd_notify(0, "READY=1") < 0) {
    perror("systemd_notify_ready: sd_notify failed");
  }


  pid = waitpid(-1, &status, 0);
  printf("child terminated: %d\n", pid);

  return (WIFEXITED(status) && WEXITSTATUS(status) == 0 ? 0 : 1);
}
