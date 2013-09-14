#include "config.h"

// if liblightgrep isn't present, compiles to nothing
#ifdef HAVE_LIBLIGHTGREP

#include <string>

#include "be13_api/bulk_extractor_i.h"
#include "histogram.h"
#include "pattern_scanner.h"

namespace base16 {
//  const char* const DefaultEncodingsCStrings[] = {"UTF-8", "UTF-16LE"};
  const char* const DefaultEncodingsCStrings[] = {"UTF-8"};

  const vector<string> DefaultEncodings(
    &DefaultEncodingsCStrings[0],
    &DefaultEncodingsCStrings[0] +
      sizeof(DefaultEncodingsCStrings)/sizeof(DefaultEncodingsCStrings[0])
  );

  const LG_KeyOptions DefaultOptions = { 0, 1 }; // patterns, case-insensitive

  //
  // the scanner
  //

  const int BASE16_INVALID = -1;
  const int BASE16_IGNORE = -2;

  class Scanner: public PatternScanner {
  public:
    Scanner(): PatternScanner("base16_lg"), Recorder(0) {}
    virtual ~Scanner() {}

    virtual Scanner* clone() const { return new Scanner(*this); }

    virtual void startup(const scanner_params& sp);
    virtual void init(const scanner_params& sp);
    virtual void initScan(const scanner_params&);

    feature_recorder* Recorder;

    void hitHandler(const LG_SearchHit& hit, const scanner_params& sp, const recursion_control_block& rcb) {
      decode(sp.sbuf, hit.Start + 1, hit.End - hit.Start - 2, sp, rcb);
    }

  private:
    Scanner(const Scanner& s):
      PatternScanner(s),
      Recorder(s.Recorder)
    {}

    Scanner& operator=(const Scanner&);

    void decode(const sbuf_t& osbuf, size_t pos, size_t len, const scanner_params& sp, const recursion_control_block& rcb);
  };

  const int BASE16[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -2, -2, -1, -1, -2, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
  };

  const uint32_t BASE16_LSN[256] = {
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
      0,   1,   2,   3,   4,   5,   6,   7,
      8,   9, 256, 256, 256, 256, 256, 256,
    256,  10,  11,  12,  13,  14,  15, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256,  10,  11,  12,  13,  14,  15, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256
  };

  const uint32_t BASE16_MSN[256] = {
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
      0,  16,  32,  48,  64,  80,  96, 112,
    128, 144, 256, 256, 256, 256, 256, 256,
    256, 160, 176, 192, 208, 224, 240, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 160, 176, 192, 208, 224, 240, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 556, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 256, 256, 256, 256, 256
  };

  void Scanner::startup(const scanner_params& sp) {
    assert(sp.sp_version == scanner_params::CURRENT_SP_VERSION);
    assert(sp.info->si_version == scanner_info::CURRENT_SI_VERSION);

    sp.info->name             = "base16_lg";
    sp.info->author           = "Simson L. Garfinkel";
    sp.info->description      = "Base16 (hex) scanner";
    sp.info->scanner_version  = "1.0";
    sp.info->flags            = scanner_info::SCANNER_RECURSE;

    sp.info->feature_names.insert("hex"); // notable hex values
  }

  void Scanner::init(const scanner_params& sp) {
    //
    // patterns
    //

    // FIXME: trailing context
    // FIXME: leading context
    /*
     * hex with junk before it.
     * {0,4} means we have 0-4 space characters
     * {6,}  means minimum of 6 hex bytes
     */
     const std::string HEX("[^0-9A-F]([0-9A-F]{2}[ \\t\\n\\r]{0,4}){6,}[^0-9A-F]");

    new Handler(
      *this,
      HEX,
      DefaultEncodings,
      DefaultOptions,
      static_cast<CallbackFnType>(&Scanner::hitHandler)
    );
  }

  void Scanner::initScan(const scanner_params& sp) {
    Recorder = sp.fs.get_name("hex");
  }

  // Don't re-analyze hex bufs smaller than this
  const unsigned int opt_min_hex_buf = 64;

/*
  size_t base16_decode_skipping_whitespace(const uint8_t* dst_start, const char* src, const char* src_end) {
    uint8_t* dst = dst_start;
    uint32_t byte;

    while (src < src_end) {
      byte = BASE16_MSN[*src++] | BASE16_LSN[*src++];
      if (byte > 0xFF) {
        return dst - dst_start;
      }
      *dst++ = static_cast<uint8_t>(byte);
    }

    return dst - dst_startsrc_end - src;
  }
*/

  void Scanner::decode(const sbuf_t& osbuf, size_t pos, size_t len, const scanner_params& sp, const recursion_control_block& rcb) {
    sbuf_t sbuf(osbuf, pos, len);  // the substring we are working with

/*
    const size_t outbytes = sbuf.pagesize/2;
    managed_malloc<uint8_t> b(outbytes);

    size_t p = 0;
    for (size_t i = 0; i < sbuf.pagesize; ) {
      size_t dlen = base16_decode(&b + p, sbuf.buf + i, sbuf.pagesize);
      if (!dlen) {
        ++i;
        continue;
      }

      p += dlen;
      i += (dlen << 1);
    }
*/

    managed_malloc<uint8_t> b(sbuf.pagesize/2);
    if (b.buf == 0) return;

    size_t p = 0;
    // First get the characters
    for (size_t i = 0; i+1 < sbuf.pagesize; ) {
      // stats on the new characters

      // decode the two characters
      const int msb = BASE16[sbuf[i]];
      if (msb == BASE16_IGNORE || msb == BASE16_INVALID) {
        i++;          // This character not valid
        continue;
      }

      assert(msb >= 0 && msb < 16);
      const int lsb = BASE16[sbuf[i+1]];
      if (lsb == BASE16_IGNORE || lsb == BASE16_INVALID) {
        // If first char is valid hex and second isn't, this isn't hex
        return;
      }

      assert(lsb >= 0 && lsb < 16);
      b.buf[p++] = (msb<<4) | lsb;
      i+=2;
    }

    // Alert on byte sequences of 48, 128 or 256 bits
    if (p == 48/8 || p == 128/8 || p == 256/8) {
      // it validates; write original with context
      Recorder->write_buf(osbuf, pos, len);
      return; // Small keys don't get recursively analyzed
    }

    if (p > opt_min_hex_buf){
      sbuf_t nsbuf(sbuf.pos0, b.buf, p, p, false);
      (*rcb.callback)(scanner_params(sp, nsbuf)); // recurse
    }
  }

  Scanner TheScanner;
}

extern "C"
void scan_base16_lg(const class scanner_params &sp, const recursion_control_block &rcb) {
  scan_lg(base16::TheScanner, sp, rcb);
}

#endif // HAVE_LIBLIGHTGREP
