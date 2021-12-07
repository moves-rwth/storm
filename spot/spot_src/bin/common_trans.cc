// -*- coding: utf-8 -*-
// Copyright (C) 2015-2020 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "common_trans.hh"
#include "common_setup.hh"
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <iomanip>
#ifdef HAVE_SPAWN_H
#include <spawn.h>
#endif

#include "error.h"

#include <spot/tl/print.hh>
#include <spot/tl/unabbrev.hh>
#include "common_conv.hh"
#include <spot/misc/escape.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/twaalgos/lbtt.hh>
#include <spot/twaalgos/neverclaim.hh>

// A set of tools for which we know the correct output
struct shorthands_t
{
  const char* prefix;
  const char* suffix;
};
static shorthands_t shorthands_ltl[] = {
    { "delag", " %f>%O" },
    { "lbt", " <%L>%O" },
    { "ltl2ba", " -f %s>%O" },
    { "ltl2da", " %f>%O" },
    { "ltl2dgra", " %f>%O" },
    { "ltl2dpa", " %f>%O" },
    { "ltl2dra", " %f>%O" },
    { "ltl2dstar", " --output-format=hoa %[MW]L %O"},
    { "ltl2ldba", " %f>%O" },
    { "ltl2na", " %f>%O" },
    { "ltl2nba", " %f>%O" },
    { "ltl2ngba", " %f>%O" },
    { "ltl2tgba", " -H %f>%O" },
    { "ltl3ba", " -f %s>%O" },
    { "ltl3dra", " -f %s>%O" },
    { "ltl3hoa", " -f %f>%O" },
    { "ltl3tela", " -f %f>%O" },    // ltl3tela is the new name of ltl3hoa
    { "modella", " %[MWei^]L %O" },
    { "spin", " -f %s>%O" },
  };

static shorthands_t shorthands_autproc[] = {
    { "autfilt", " %H>%O" },
    { "dra2dpa", " <%H>%O" },
    { "dstar2tgba", " %H>%O" },
    { "ltl2dstar", " -B %H %O" },
    { "nba2dpa", " <%H>%O" },
    { "nba2ldpa", " <%H>%O" },
    { "seminator", " %H>%O" },
  };

static void show_shorthands(shorthands_t* begin, shorthands_t* end)
{
  std::cout
    << ("If a COMMANDFMT does not use any %-sequence, and starts with one of\n"
        "the following words, then the string on the right is appended.\n\n");
  while (begin != end)
    {
      std::cout << "  "
                << std::left << std::setw(12) << begin->prefix
                << begin->suffix << '\n';
      ++begin;
    }
}


tool_spec::tool_spec(const char* spec, shorthands_t* begin, shorthands_t* end,
                     bool is_ref) noexcept
  : spec(spec), cmd(spec), name(spec), reference(is_ref)
{
  if (*cmd == '{')
    {
      // Match the closing '}'
      const char* pos = cmd;
      unsigned count = 1;
      while (*++pos)
        {
          if (*pos == '{')
            ++count;
          else if (*pos == '}')
            if (!--count)
              {
                name = strndup(cmd + 1, pos - cmd - 1);
                cmd = pos + 1;
                while (*cmd == ' ' || *cmd == '\t')
                  ++cmd;
                break;
              }
        }
    }
  // If there is no % in the string, look for a known
  // command from our shorthand list.  If we find it,
  // add the suffix.
  bool allocated = false;
  if (!strchr(cmd, '%'))
    {
      // Skip any leading directory name.
      auto basename = cmd;
      auto pos = cmd;
      while (*pos)
        {
          if (*pos == '/')
            basename = pos + 1;
          else if (*pos == ' ')
            break;
          ++pos;
        }
      // Match a shorthand.
      while (begin != end)
        {
          auto& p = *begin++;
          int n = strlen(p.prefix);
          if (strncmp(basename, p.prefix, n) == 0)
            {
              int m = strlen(p.suffix);
              int q = strlen(cmd);
              char* tmp = static_cast<char*>(malloc(q + m + 1));
              strcpy(tmp, cmd);
              strcpy(tmp + q, p.suffix);
              cmd = tmp;
              allocated = true;
              break;
            }
        }
    }
  if (!allocated)
    cmd = strdup(cmd);
}

tool_spec::tool_spec(const tool_spec& other) noexcept
  : spec(other.spec), cmd(other.cmd), name(other.name),
    reference(other.reference)
{
  if (cmd != spec)
    cmd = strdup(cmd);
  if (name != spec)
    name = strdup(name);
}

tool_spec& tool_spec::operator=(const tool_spec& other)
{
  spec = other.spec;
  cmd = other.cmd;
  if (cmd != spec)
    cmd = strdup(cmd);
  name = other.name;
  if (name != spec)
    name = strdup(name);
  return *this;
}

tool_spec::~tool_spec()
{
  if (name != spec)
    free(const_cast<char*>(name));
  if (cmd != spec)
    free(const_cast<char*>(cmd));
}

std::vector<tool_spec> tools;

void tools_push_trans(const char* trans, bool is_ref)
{
  tools.emplace_back(trans,
                     std::begin(shorthands_ltl),
                     std::end(shorthands_ltl),
                     is_ref);
}

void tools_push_autproc(const char* proc, bool is_ref)
{
  tools.emplace_back(proc,
                     std::begin(shorthands_autproc),
                     std::end(shorthands_autproc),
                     is_ref);
}

void quoted_formula::print(std::ostream& os, const char* pos) const
{
  spot::formula f = val_;
  if (*pos == '[')
    {
      ++pos;
      auto end = strchr(pos, ']');
      auto arg = strndup(pos, end - pos);
      f = spot::unabbreviate(f, arg);
      free(arg);
      pos = end + 1;
    }
  std::ostringstream ss;
  std::ostream* out = &ss;
  bool quoted = true;
  switch (*pos)
    {
    case 'F':
    case 'S':
    case 'L':
    case 'W':
      out = &os;
      quoted = false;
    }
  switch (*pos)
    {
    case 'f':
    case 'F':
      print_psl(*out, f, true);
      break;
    case 's':
    case 'S':
      print_spin_ltl(*out, f, true);
      break;
    case 'l':
    case 'L':
      print_lbt_ltl(*out, f);
      break;
    case 'w':
    case 'W':
      print_wring_ltl(*out, f);
      break;
    }
  if (quoted)
    {
      std::string s = ss.str();
      spot::quote_shell_string(os, s.c_str());
    }
}



printable_result_filename::printable_result_filename()
{
  val_ = nullptr;
}

printable_result_filename::~printable_result_filename()
{
  delete val_;
}

void printable_result_filename::reset(unsigned n)
{
  translator_num = n;
}

void printable_result_filename::cleanup()
{
  delete val_;
  val_ = nullptr;
}

void
printable_result_filename::print(std::ostream& os, const char*) const
{
  char prefix[30];
  snprintf(prefix, sizeof prefix, "lcr-o%u-", translator_num);
  const_cast<printable_result_filename*>(this)->val_ =
    spot::create_tmpfile(prefix);
  spot::quote_shell_string(os, val()->name());
}

static std::string
string_to_tmp(const std::string str, unsigned n)
{
  char prefix[30];
  snprintf(prefix, sizeof prefix, "lcr-i%u-", n);
  spot::open_temporary_file* tmpfile = spot::create_open_tmpfile(prefix);
  std::string tmpname = tmpfile->name();
  int fd = tmpfile->fd();
  ssize_t s = str.size();
  if (write(fd, str.c_str(), s) != s
      || write(fd, "\n", 1) != 1)
    error(2, errno, "failed to write into %s", tmpname.c_str());
  tmpfile->close();
  return tmpname;
}

void
filed_formula::print(std::ostream& os, const char* pos) const
{
  std::ostringstream ss;
  f_.print(ss, pos);
  os << '\'' << string_to_tmp(ss.str(), serial_) << '\'';
}

void
filed_automaton::print(std::ostream& os, const char* pos) const
{
  std::ostringstream ss;
  char* opt = nullptr;
  bool filename = true;
  if (*pos == '[')
    {
      ++pos;
      auto end = strchr(pos, ']');
      opt = strndup(pos, end - pos);
      pos = end + 1;
    }
  switch (*pos)
    {
    case 'H':
      spot::print_hoa(ss, aut_, opt);
      break;
    case 'S':
      spot::print_never_claim(ss, aut_, opt);
      break;
    case 'L':
      spot::print_lbtt(ss, aut_, opt);
      break;
    case 'M':
      {
        filename = false;
        std::string* name = aut_->get_named_prop<std::string>("automaton-name");
        spot::quote_shell_string(os,
                                 name ? name->c_str() :
                                 opt ? opt :
                                 "");
      }
    }
  if (opt)
    free(opt);
  if (filename)
    os << '\'' << string_to_tmp(ss.str(), serial_) << '\'';
}

translator_runner::translator_runner(spot::bdd_dict_ptr dict,
                                     bool no_output_allowed)
  : dict(dict)
{
  declare('f', &ltl_formula);
  declare('s', &ltl_formula);
  declare('l', &ltl_formula);
  declare('w', &ltl_formula);
  declare('F', &filename_formula);
  declare('S', &filename_formula);
  declare('L', &filename_formula);
  declare('W', &filename_formula);
  declare('D', &output);
  declare('H', &output);
  declare('N', &output);
  declare('T', &output);
  declare('O', &output);

  size_t s = tools.size();
  assert(s);
  for (size_t n = 0; n < s; ++n)
    {
      // Check that each translator uses at least one input and
      // one output.
      std::vector<bool> has(256);
      const tool_spec& t = tools[n];
      scan(t.cmd, has);
      if (!(has['f'] || has['s'] || has['l'] || has['w']
            || has['F'] || has['S'] || has['L'] || has['W']))
        error(2, 0, "no input %%-sequence in '%s'.\n       Use "
              "one of %%f,%%s,%%l,%%w,%%F,%%S,%%L,%%W to indicate how "
              "to pass the formula.", t.spec);
      if (!no_output_allowed
          && !(has['O'] ||
               // backward-compatibility
               has['D'] || has['N'] || has['T'] || has['H']))
        error(2, 0, "no output %%-sequence in '%s'.\n      Use  "
              "%%O to indicate where the automaton is output.",
              t.spec);
      // Remember the %-sequences used by all tools.
      prime(t.cmd);
    }
}

std::string
translator_runner::formula() const
{
  // Pick the most readable format we have...
  if (has('f') || has('F'))
    return spot::str_psl(ltl_formula, true);
  if (has('s') || has('S'))
    return spot::str_spin_ltl(ltl_formula, true);
  if (has('l') || has('L'))
    return spot::str_lbt_ltl(ltl_formula);
  if (has('w') || has('W'))
    return spot::str_wring_ltl(ltl_formula);
  SPOT_UNREACHABLE();
  return spot::str_psl(ltl_formula, true);
}

void
translator_runner::round_formula(spot::formula f, unsigned serial)
{
  ltl_formula = f;
  filename_formula.new_round(serial);
}


autproc_runner::autproc_runner(bool no_output_allowed)
{
  declare('H', &filename_automaton);
  declare('S', &filename_automaton);
  declare('L', &filename_automaton);
  declare('M', &filename_automaton);
  declare('O', &output);

  size_t s = tools.size();
  assert(s);
  for (size_t n = 0; n < s; ++n)
    {
      // Check that each translator uses at least one input and
      // one output.
      std::vector<bool> has(256);
      const tool_spec& t = tools[n];
      scan(t.cmd, has);
      if (!(has['H'] || has['S'] || has['L'] || has['M']))
        error(2, 0, "no input %%-sequence in '%s'.\n       Use "
              "one of %%H,%%S,%%L to indicate the input automaton filename.\n"
              "        (Or use %%M if you want to work from its name.)\n",
              t.spec);
      if (!no_output_allowed && !has['O'])
        error(2, 0, "no output %%-sequence in '%s'.\n      Use  "
              "%%O to indicate where the automaton is output.",
              t.spec);
      // Remember the %-sequences used by all tools.
      prime(t.cmd);
    }
}

void
autproc_runner::round_automaton(spot::const_twa_graph_ptr aut, unsigned serial)
{
  filename_automaton.new_round(aut, serial);
}

std::atomic<bool> timed_out{false};
unsigned timeout_count = 0;

static unsigned timeout = 0;
#if ENABLE_TIMEOUT
static std::atomic<int> alarm_on{0};
static int child_pid = -1;

static void
sig_handler(int sig)
{
  if (child_pid == 0)
    error(2, 0, "received signal %d before starting child", sig);

  if (sig == SIGALRM && alarm_on)
    {
      timed_out = true;
      if (--alarm_on)
        {
          // Send SIGTERM to children.
          kill(-child_pid, SIGTERM);
          // Try again later if it didn't work.  (alarm() will be reset
          // if it did work and the call to wait() returns)
          alarm(2);
        }
      else
        {
          // After a few gentle tries, really kill that child.
          kill(-child_pid, SIGKILL);
        }
    }
  else
    {
      // forward signal
      kill(-child_pid, sig);
      // cleanup files
      spot::cleanup_tmpfiles();
      // and die verbosely
      error(2, 0, "received signal %d", sig);
    }
}

void
setup_sig_handler()
{
  struct sigaction sa;
  sa.sa_handler = sig_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART; // So that wait() doesn't get aborted by SIGALRM.
  sigaction(SIGALRM, &sa, nullptr);
  // Catch termination signals, so we can kill the subprocess.
  sigaction(SIGHUP, &sa, nullptr);
  sigaction(SIGINT, &sa, nullptr);
  sigaction(SIGQUIT, &sa, nullptr);
  sigaction(SIGTERM, &sa, nullptr);
}


static char*
get_arg(const char*& cmd)
{
  const char* start = cmd;
  std::string arg;
  while (int c = *cmd)
    {
      switch (c)
        {
          // Those characters can have special meaning for the shell.
        case '`':
        case '~':
        case '|':
        case ';':
        case '!':
        case '?':
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
        case '$':
        case '*':
        case '&':
        case '#':
        case '\\':
        case '>':
        case '<':
        case ' ':
        case '\n':
        case '\t':
          goto end_loop;
        case '\'':
          {
            int d = 0;
            while ((d = *++cmd))
              {
                if (d == '\'')
                  break;
                arg.push_back(d);
              }
            if (d == 0)
              return nullptr;
          }
          break;
        case '"':
          {
            int d = 0;
            while ((d = *++cmd))
              {
                if (d == '\"')
                  break;
                // Backslash can only be used to escape \, $, `, and "
                if (d == '\\' && strchr("\\$`\"", *cmd))
                  d = *++cmd;
                else if (strchr("\\$`", d))
                  return nullptr;
                arg.push_back(d);
              }
            if (d == 0)
              return nullptr;
          }
          break;
        default:
          arg.push_back(c);
          break;
        }
      ++cmd;
    }
 end_loop:
  if (cmd == start)             // Not the same as arg.empty()
    return nullptr;
  return strndup(arg.c_str(), arg.size());
}

static void
skip_ws(const char*& cmd)
{
  while (isspace(*cmd))
    ++cmd;
}


// Commands are run via 'sh -c' so we get all the expressive power of
// the shell.  However starting a shell for each translation is slow.
// To mitigate that, if the command to run is simple: we run it
// directly, bypassing the shell.   Our definition of simple is:
//   - a single command
//   - can have single or double-quoted arguments
//   - can have >stderr and <stdin redirection
// In particular, variable interpolation is not supported.  Complex
// redirections (>& and such) are not support.  Chains of commands
// (pipes, semi-colons, etc.) are not supported.  Envvar definitions
// before the command are not supported.

struct simple_command
{
  std::vector<char*> args;
  char* stdin_filename = nullptr;
  char* stdout_filename = nullptr;

  simple_command(const simple_command& other) = delete;
  simple_command& operator=(const simple_command& other) = delete;
  simple_command() = default;
  simple_command(simple_command&& other) = default;

  void clear()
  {
    for (auto arg: args)
      free(arg);
    args.clear();
    free(stdin_filename);
    free(stdout_filename);
    stdin_filename = nullptr;
    stdout_filename = nullptr;
  }

  ~simple_command()
  {
    clear();
  }
};

static simple_command
parse_simple_command(const char* cmd)
{
  simple_command res;
  const char* start = cmd;

  while (*cmd)
    {
      skip_ws(cmd);

      switch (*cmd)
        {
        case '<':
          {
            if (cmd > start && isdigit(cmd[-1]))
              goto use_shell;
            ++cmd;
            skip_ws(cmd);
            if (res.stdin_filename)
              free(res.stdin_filename);
            res.stdin_filename = get_arg(cmd);
            if (res.stdin_filename == nullptr)
              goto use_shell;
            break;
          }
        case '>':
          {
            if (cmd > start && isdigit(cmd[-1]))
              goto use_shell;
            ++cmd;
            skip_ws(cmd);
            if (res.stdout_filename)
              free(res.stdout_filename);
            res.stdout_filename = get_arg(cmd);
            if (res.stdout_filename == nullptr)
              goto use_shell;
            break;
          }
        default:
          {
            char* tmp = get_arg(cmd);
            if (tmp == nullptr)
              goto use_shell;
            res.args.push_back(tmp);
            break;
          }
        }
    }
  // If result is empty, we failed to found the command; let's see if
  // the shell is smarter.  If the command contains '=', it's unlikely
  // to be a command, but probably an environment variable definition
  // as in
  //    FOO=1 command args..
  if (res.args.empty() || strchr(res.args[0], '='))
    goto use_shell;
  res.args.push_back(nullptr);
  return res;

 use_shell:
  res.clear();
  return res;
}

#ifndef HAVE_SPAWN_H
static void
exec_command(const char* cmd)
{
  simple_command res = parse_simple_command(cmd);

  if (!res.args.empty())
    {
      if (res.stdin_filename)
        {
          int fd0 = open(res.stdin_filename, O_RDONLY, 0644);
          if (fd0 < 0)
            error(2, errno, "failed to open '%s'", res.stdin_filename);
          if (dup2(fd0, STDIN_FILENO) < 0)
            error(2, errno, "dup2() failed");
          if (close(fd0) < 0)
            error(2, errno, "close() failed");
        }
      if (res.stdout_filename)
        {
          int fd1 = creat(res.stdout_filename, 0644);
          if (fd1 < 0)
            error(2, errno, "failed to open '%s'", res.stdout_filename);
          if (dup2(fd1, STDOUT_FILENO) < 0)
            error(2, errno, "dup2() failed");
          if (close(fd1) < 0)
            error(2, errno, "close() failed");
        }

      execvp(res.args[0], res.args.data());
      error(2, errno, "failed to run '%s'", res.args[0]);
      SPOT_UNREACHABLE();
      return;
    }
  // Try /bin/sh first, because it is faster to not do any PATH
  // lookup.
  static bool has_bin_sh = true;
  if (has_bin_sh)
    execl("/bin/sh", "sh", "-c", cmd, nullptr);
  has_bin_sh = false;
  execlp("sh", "sh", "-c", cmd, nullptr);
  error(2, errno, "failed to run '%s' via 'sh'", cmd);
  SPOT_UNREACHABLE();
  return;
}
#else
extern char **environ;
#endif

int
exec_with_timeout(const char* cmd)
{
  int status;

  timed_out = false;

#ifdef HAVE_SPAWN_H
  simple_command res = parse_simple_command(cmd);

  // Using posix_spawn should be preferred over fork()/exec().  It can
  // be implemented in more systems.  Since 2016, posix_spawn()
  // should be faster than fork()/exec() on Linux.  See also
  // https://www.microsoft.com/en-us/research/publication/a-fork-in-the-road/
  posix_spawnattr_t attr;
  if (int err = posix_spawnattr_init(&attr))
    error(2, err, "posix_spawnattr_init() failed");
  if (int err = posix_spawnattr_setpgroup(&attr, 0))
    error(2, err, "posix_spawnattr_setpgroup() failed");
  if (int err = posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETPGROUP))
    error(2, err, "posix_spawnattr_setflags() failed");
  posix_spawn_file_actions_t actions;
  if (int err = posix_spawn_file_actions_init(&actions))
    error(2, err, "posix_spawn_file_actions_init() failed");
  // Close stdin so that children may not read our input.  We had this
  // nice surprise with Seminator, who greedily consumes its stdin
  // (which was also ours) even if it does not use it because it was
  // given a file.
  if (int err = posix_spawn_file_actions_addclose(&actions, STDIN_FILENO))
    error(2, err, "posix_spawn_file_actions_addclose() failed");
  if (!res.args.empty())
    {
      if (res.stdin_filename)
        if (int err = posix_spawn_file_actions_addopen(&actions, STDIN_FILENO,
                                                       res.stdin_filename,
                                                       O_RDONLY, 0644))
          error(2, err, "posix_spawn_file_actions_addopen() failed");
      if (res.stdout_filename)
        if (int err = posix_spawn_file_actions_addopen(&actions, STDOUT_FILENO,
                                                       res.stdout_filename,
                                                       O_CREAT | O_WRONLY |
                                                       O_TRUNC, 0644))
          error(2, err, "posix_spawn_file_actions_addopen() failed");
      if (int err = posix_spawnp(&child_pid, res.args[0], &actions, &attr,
                                 res.args.data(), environ))
        error(2, err, "failed to run '%s'", res.args[0]);
    }
  else
    {
      // Try /bin/sh first, because it is faster to not do any PATH
      // lookup.
      static bool has_bin_sh = true;
      if (has_bin_sh)
        {
          const char* args[] = { "/bin/sh", "-c", cmd, nullptr };
          if (posix_spawn(&child_pid, args[0], &actions, &attr,
                          const_cast<char **>(args), environ))
            has_bin_sh = false;
        }
      if (!has_bin_sh)
        {
          const char* args[] = { "sh", "-c", cmd, nullptr };
          if (int err = posix_spawnp(&child_pid, args[0], &actions, &attr,
                                     const_cast<char **>(args), environ))
            error(2, err, "failed to run '%s' via 'sh'", cmd);
        }
    }
  if (int err = posix_spawn_file_actions_destroy(&actions))
    error(2, err, "posix_spawn_file_actions_destroy() failed");
  if (int err = posix_spawnattr_destroy(&attr))
    error(2, err, "posix_spawnattr_destroy() failed");
#else
  child_pid = fork();
  if (child_pid == -1)
    error(2, errno, "failed to fork()");

  if (child_pid == 0)
    {
      setpgid(0, 0);
      // Close stdin so that children may not read our input.  We had
      // this nice surprise with Seminator, who greedily consumes its
      // stdin (which was also ours) even if it does not use it
      // because it was given a file.
      close(STDIN_FILENO);
      exec_command(cmd);
      // never reached
      return -1;
    }
#endif
  alarm(timeout);
  // Upon SIGALRM, the child will receive up to 3
  // signals: SIGTERM, SIGTERM, SIGKILL.
  alarm_on = 3;
  int w = waitpid(child_pid, &status, 0);
  alarm_on = 0;

  if (w == -1)
    error(2, errno, "error during wait()");

  alarm(0);
  return status;
}
#endif // ENABLE_TIMEOUT

enum {
  OPT_LIST = 1,
  OPT_RELABEL = 2,
};
static const argp_option options[] =
{
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Specifying translators to call:", 2 },
    { "translator", 't', "COMMANDFMT", 0,
      "register one translator to call", 0 },
    { "timeout", 'T', "NUMBER", 0, "kill translators after NUMBER seconds", 0 },
    { "list-shorthands", OPT_LIST, nullptr, 0,
      "list availabled shorthands to use in COMMANDFMT", 0},
    { "relabel", OPT_RELABEL, nullptr, 0,
      "always relabel atomic propositions before calling any translator", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0,
      "COMMANDFMT should specify input and output arguments using the "
      "following character sequences:", 3 },
    { "%f,%s,%l,%w", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the formula as a (quoted) string in Spot, Spin, LBT, or Wring's syntax",
      0 },
    { "%F,%S,%L,%W", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the formula as a file in Spot, Spin, LBT, or Wring's syntax", 0 },
    { "%O", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the automaton output in HOA, never claim, LBTT, or ltl2dstar's "
      "format", 0 },
    { "%%", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE, "a single %", 0 },
    { nullptr, 0, nullptr, 0,
      "If either %l, %L, or %T are used, any input formula that does "
      "not use LBT-style atomic propositions (i.e. p0, p1, ...) will be "
      "relabeled automatically.  Likewise if %s or %S are used with "
      "atomic proposition that compatible with Spin's syntax.  You can "
      "force this relabeling to always occur with option --relabel.\n"
      "The sequences %f,%s,%l,%w,%F,%S,%L,%W can optionally be \"infixed\""
      " by a bracketed sequence of operators to unabbreviate before outputing"
      " the formula.  For instance %[MW]f will rewrite operators M and W"
      " before outputing it.\n"
      "Furthermore, if COMMANDFMT has the form \"{NAME}CMD\", then only CMD "
      "will be passed to the shell, and NAME will be used to name the tool "
      "in the output.", 4 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
};

bool opt_relabel = false;

static int parse_opt_trans(int key, char* arg, struct argp_state*)
{
  // Called from C code, so should not raise any exception.
  BEGIN_EXCEPTION_PROTECT;
  switch (key)
    {
    case 't':
      tools_push_trans(arg);
      break;
    case 'T':
      timeout = to_pos_int(arg, "-T/--timeout");
#if !ENABLE_TIMEOUT
      std::cerr << "warning: setting a timeout is not supported "
                << "on your platform" << std::endl;
#endif
      break;
    case OPT_LIST:
      show_shorthands(std::begin(shorthands_ltl), std::end(shorthands_ltl));
      std::cout
        << ("\nAny {name} and directory component is skipped for the purpose "
            "of\nmatching those prefixes.  So for instance\n  "
            "'{DRA} ~/mytools/ltl2dstar-0.5.2'\n"
            "will be changed into\n  "
            "'{DRA} ~/mytools/ltl2dstar-0.5.2 --output-format=hoa %[MW]L %O'"
            "\n");
      exit(0);
    case OPT_RELABEL:
      opt_relabel = true;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  END_EXCEPTION_PROTECT;
  return 0;
}

const struct argp trans_argp = { options, parse_opt_trans, nullptr, nullptr,
                                 nullptr, nullptr, nullptr };



static const argp_option options_aut[] =
{
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Specifying tools to call:", 2 },
    { "tool", 't', "COMMANDFMT", 0,
      "register one tool to call", 0 },
    { "timeout", 'T', "NUMBER", 0, "kill tools after NUMBER seconds", 0 },
    { "list-shorthands", OPT_LIST, nullptr, 0,
      "list availabled shorthands to use in COMMANDFMT", 0},
    /**************************************************/
    { nullptr, 0, nullptr, 0,
      "COMMANDFMT should specify input and output arguments using the "
      "following character sequences:", 3 },
    { "%H,%S,%L", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "filename for the input automaton in (H) HOA, (S) Spin's neverclaim, "
      "or (L) LBTT's format", 0 },
    { "%M, %[val]M", 0, nullptr,  OPTION_DOC | OPTION_NO_USAGE,
      "the name of the input automaton, with an optional default value", 0 },
    { "%O", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "filename for the automaton output in HOA, never claim, LBTT, or "
      "ltl2dstar's format", 0 },
    { "%%", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE, "a single %", 0 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
};

static int parse_opt_autproc(int key, char* arg, struct argp_state*)
{
  switch (key)
    {
    case 't':
      tools_push_autproc(arg);
      break;
    case 'T':
      timeout = to_pos_int(arg, "-T/--timeout");
#if !ENABLE_TIMEOUT
      std::cerr << "warning: setting a timeout is not supported "
                << "on your platform" << std::endl;
#endif
      break;
    case OPT_LIST:
      show_shorthands(std::begin(shorthands_autproc),
                      std::end(shorthands_autproc));
      std::cout
        << ("\nAny {name} and directory component is skipped for the purpose "
            "of\nmatching those prefixes.  So for instance\n  "
            "'{AF} ~/mytools/autfilt-2.4 --remove-fin'\n"
            "will be changed into\n  "
            "'{AF} ~/mytools/autfilt-2.4 --remove-fin %H>%O'\n");
      exit(0);
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

const struct argp autproc_argp = { options_aut, parse_opt_autproc, nullptr,
                                   nullptr, nullptr, nullptr, nullptr };
