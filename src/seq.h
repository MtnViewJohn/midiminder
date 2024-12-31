#pragma once

// Manage being an ALSA Sequencer client

#include <alsa/asoundlib.h>
#include <fmt/format.h>
#include <functional>
#include <iostream>
#include <string>
#include <unicode/unistr.h>


class Address {
  public:
    Address()
      : valid(false), addr{0, 0}
      { }
    Address(const snd_seq_addr_t& a, bool m, unsigned int f, unsigned int t,
        const std::string& c, const std::string& p);

    // allow copying
    Address(const Address&) = default;
    Address& operator=(const Address&) = default;

    static const Address null;

    explicit operator bool() const { return valid; }

    bool canBeSender() const { return valid && caps & SND_SEQ_PORT_CAP_SUBS_READ; }
    bool canBeDest() const   { return valid && caps & SND_SEQ_PORT_CAP_SUBS_WRITE; }

    bool matches(const snd_seq_addr_t& a) const
      { return valid && addr.client == a.client && addr.port == a.port; }

    fmt::format_context::iterator format(fmt::format_context&) const;
    std::string capsString() const;
    std::string typeString() const;

    bool valid;
    bool mindable;
    snd_seq_addr_t addr;
    unsigned int caps;
    unsigned int types;
    std::string client;
    std::string port;
    std::string portLong;
    icu::UnicodeString clientFolded;
    icu::UnicodeString portFolded;
    bool primarySender;
    bool primaryDest;
};

using client_id_t = unsigned char;

class Seq {
  public:
    Seq() { }

    void begin(const char* clientName);
    void end();
    explicit operator bool() const { return seq; }

    std::string clientName(client_id_t);
    Address address(const snd_seq_addr_t&);

    void scanFDs(std::function<void(int)>);
    snd_seq_event_t * eventInput();
      // if nullptr is returned, sleep and call again...

    void scanClients(std::function<void(client_id_t)>);
    void scanPorts(std::function<void(const snd_seq_addr_t&)>);
    void scanConnections(std::function<void(const snd_seq_connect_t&)>);
    void connect(const snd_seq_addr_t& sender, const snd_seq_addr_t& dest);
    void disconnect(const snd_seq_connect_t& conn);

    bool errCheck(int serr, const char* op);
    bool errFatal(int serr, const char* op);

  private:
    snd_seq_t *seq = nullptr;
    client_id_t seqClient;
    int evtPort;

  public:
    static void outputAddr(std::ostream&, const snd_seq_addr_t&);
    static void outputConnect(std::ostream&, const snd_seq_connect_t&);
    static void outputEvent(std::ostream& out, const snd_seq_event_t& ev);

    std::string clientDetails(client_id_t);
    bool isThisClient(client_id_t c) { return c == seqClient; }
    bool isMindableClient(client_id_t c) const;
};



template <> struct fmt::formatter<Address> : formatter<string_view> {
  auto format(const Address& a, format_context& ctx) const
    { return a.format(ctx); }
};

template <> struct fmt::formatter<snd_seq_addr_t> : formatter<string_view> {
  format_context::iterator
  format(const snd_seq_addr_t &, format_context &) const;
};

template <> struct fmt::formatter<snd_seq_connect_t> : formatter<string_view> {
  format_context::iterator
  format(const snd_seq_connect_t &, format_context &) const;
};

template <> struct fmt::formatter<snd_seq_event_t> : formatter<string_view> {
  format_context::iterator
  format(const snd_seq_event_t &, format_context &) const;
};


inline std::ostream& operator<<(std::ostream& s, const Address& a)
  { s << fmt::format("{}", a); return s; }

inline std::ostream& operator<<(std::ostream& s, const snd_seq_addr_t& a)
  { s << fmt::format("{}", a); return s; }

inline std::ostream& operator<<(std::ostream& s, const snd_seq_connect_t& c)
  { s << fmt::format("{}", c); return s; }

inline std::ostream& operator<<(std::ostream& s, const snd_seq_event_t& ev)
  { s << fmt::format("{}", ev); return s; }


inline bool operator==(const snd_seq_addr_t& a, const snd_seq_addr_t& b) {
  return a.client == b.client && a.port == b.port;
}

inline bool operator<(const snd_seq_addr_t& a, const snd_seq_addr_t& b) {
  return a.client == b.client ? a.port < b.port : a.client < b.client;
}

inline bool operator==(const snd_seq_connect_t& a, const snd_seq_connect_t& b) {
  return a.sender == b.sender && a.dest == b.dest;
}

inline bool operator<(const snd_seq_connect_t& a, const snd_seq_connect_t& b) {
  return a.sender == b.sender ? a.dest < b.dest : a.sender < b.sender;
}
