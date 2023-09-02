//
// _RevInstTable_h_
//
// Copyright (C) 2017-2023 Tactical Computing Laboratories, LLC
// All Rights Reserved
// contact@tactcomplabs.com
//
// See LICENSE in the top level directory for licensing details
//

#ifndef _SST_REVCPU_REVINSTTABLE_H_
#define _SST_REVCPU_REVINSTTABLE_H_

#include <bitset>
#include <string>
#include <map>
#include <cstdint>
#include <cstddef>
#include <limits>
#include <type_traits>

#include "RevMem.h"
#include "RevFeature.h"

#ifndef _REV_NUM_REGS_
#define _REV_NUM_REGS_ 32
#endif

#ifndef _REV_MAX_FORMAT_
#define _REV_MAX_FORMAT_ 7
#endif

#ifndef _REV_MAX_REGCLASS_
#define _REV_MAX_REGCLASS_ 3
#endif

#ifndef _REV_HART_COUNT_
#define _REV_HART_COUNT_ 1
#endif

#ifndef _REV_INVALID_HART_ID_
#define _REV_INVALID_HART_ID_ (uint16_t)~(uint16_t(0))
#endif

// Masks
#define MASK8   0b11111111                          // 8bit mask
#define MASK16  0b1111111111111111                  // 16bit mask
#define MASK32  0b11111111111111111111111111111111  // 32bit mask

// Register Decoding Macros
#define DECODE_RD(x)    (((x)>>(7))&(0b11111))
#define DECODE_RS1(x)   (((x)>>(15))&(0b11111))
#define DECODE_RS2(x)   (((x)>>(20))&(0b11111))
#define DECODE_RS3(x)   (((x)>>(27))&(0b11111))
#define DECODE_IMM12(x) (((x)>>(20))&(0b111111111111))
#define DECODE_IMM20(x) (((x)>>(12))&(0b11111111111111111111))

#define DECODE_LOWER_CRS2(x) (((x)>>(2))&(0b11111))

#define DECODE_FUNCT7(x)  (((x)>>(25))&(0b1111111))
#define DECODE_FUNCT2(x)  (((x)>>(25))&(0b11))
#define DECODE_FUNCT3(x)  (((x)>>(12))&(0b111))

#define DECODE_RL(x)    (((x)>>(25))&(0b1))
#define DECODE_AQ(x)    (((x)>>(26))&(0b1))

// RV{32,64}{F,D} macros
#define FCSR_NX(x)  ((x)&(0b1))             // FCSR: NX field
#define FCSR_UF(x)  (((x)&(0b10))>>1)       // FCSR: UF field
#define FCSR_OF(x)  (((x)&(0b100))>>2)      // FCSR: OF field
#define FCSR_DZ(x)  (((x)&(0b1000))>>3)     // FCSR: DZ field
#define FCSR_NV(x)  (((x)&(0b10000))>>4)    // FCSR: NV field
#define FCSR_FRM(x) (((x)&(0b11100000))>>5) // FCSR: FRM field

#define FRM_RNE   0b000                     // Rounding mode: Round to Nearest, ties to Even
#define FRM_RTZ   0b001                     // Rounding mode: Round towards Zero
#define FRM_RDN   0b010                     // Rounding mode: Round Down (towards -INF)
#define FRM_RUP   0b011                     // Rounding mode: Round Up (towards +INF)
#define FRM_RMM   0b100                     // Rounding mode: Round to Nearest, ties to Max Magnitude

// RV{32,64} Register Operation Macros
// TODO: These should be replaced with simpler inline functions or casts

#define SEXT(r,x,b) do {\
                    (r) = ( (x) ^ ((1UL) << ((b) - 1)) ) - ((1UL) << ((b) - 1));\
                    }while(0)                // Sign extend the target register
#define ZEXT(r,x,b) do {\
                    (r) = (x) & (((1UL) << (b)) - 1);\
                    }while(0)                // Zero extend the target register

#define SEXTI(r,b)  do {\
                    (r) = ( (r) ^ ((1UL) << ((b) - 1)) ) - ((1UL) << ((b) - 1));\
                    }while(0)                // Sign extend the target register inline
#define ZEXTI(r,b)  do {\
                    (r) = (r) & (((1UL) << (b)) - 1);\
                    }while(0)                // Zero extend the target register inline

#define SEXT64(r,x,b) do {\
                    (r) = ( (x) ^ ((1ULL) << ((b) - 1)) ) - ((1ULL) << ((b) - 1));\
                    }while(0)                // Sign extend the target register
#define ZEXT64(r,x,b) do {\
                    (r) = (x) & (((1ULL) << (b)) - 1);\
                    }while(0)                // Zero extend the target register

#define SEXTI64(r,b)  do {\
                    (r) = ( (r) ^ ((1ULL) << ((b) - 1)) ) - ((1ULL) << ((b) - 1));\
                    }while(0)                // Sign extend the target register inline
#define ZEXTI64(r,b)  do {\
                    (r) = (r) & (((1ULL) << (b)) - 1);\
                    }while(0)                // Zero extend the target register inline

/// td_u32: convert u32 in two's complement to decimal
static inline uint32_t td_u32(uint32_t binary, unsigned bits){
  uint32_t tmp = binary;
  uint32_t i = 0;
  if( (binary & (1UL<<(bits-1))) > 0 ){
    // sign extend to 32 bits
    for( i=bits; i<32; i++ ){
      tmp |= (1UL<<i);
    }

    // invert all the bits
    tmp = ~tmp;

    // add 1
    tmp += 1;

    // set the sign bit
    tmp = tmp*-1;
  }
  return tmp;
}

/// td_u64: convert u64 in two's complement to decimal
static inline uint64_t td_u64(uint64_t binary, unsigned bits){
  uint64_t tmp = binary;
  uint64_t sext = 0x00ull;
  uint64_t i = 0;

  if( (binary & (1ULL<<(bits-1))) > 0 ){
    // sign extend to 64 bits
    for( i=0; i<bits; i++ ){
      sext |= (1ULL<<i);
    }
    sext = ~sext;

    tmp = tmp | sext;

    // invert all the bits
    tmp = ~tmp;

    // add 1
    tmp += 1;

    // set the sign bit
    tmp = tmp*-1;
  }
  return tmp;
}

/// dt_u32: convert u32 in decimal to two's complement
static inline uint32_t dt_u32(int32_t binary, unsigned bits){
  uint32_t tmp = binary;
  uint32_t i = 0;
  if( (binary & (1UL<<(bits-1))) > 0 ){
    // sign extend to 32 bits
    for( i=bits; i<32; i++ ){
      tmp |= (1UL<<i);
    }

    // invert all the bits
    tmp = ~tmp;

    // add 1
    tmp += 1;

    // set the sign bit
    tmp = tmp*-1;
  }
  return tmp;
}

/// td_u64: convert u64 in decimal to two's complement
static inline uint64_t dt_u64(int64_t binary, unsigned bits){
  uint64_t tmp = binary;
  uint64_t i = 0;
  if( (binary & (1ULL<<(bits-1))) > 0 ){
    // sign extend to 32 bits
    for( i=bits; i<64; i++ ){
      tmp |= (1ULL<<i);
    }

    // invert all the bits
    tmp = ~tmp;

    // add 1
    tmp += 1;

    // set the sign bit
    tmp = tmp*-1;
  }
  return tmp;
}


namespace SST{
  namespace RevCPU {

    /* Ref: RISC-V Priviledged Spec (pg. 39) */
    enum EXCEPTION_CAUSE : uint32_t {
      MISALIGNED_INST_ADDR      = 0,
      INST_ACCESS_FAULT         = 1,
      ILLEGAL_INST              = 2,
      BREAKPOINT                = 3,
      LOAD_ADDR_MISALIGNED      = 4,
      LOAD_ACCESS_FAULT         = 5,
      STORE_AMO_ADDR_MISALIGNED = 6,
      STORE_AMO_ACCESS_FAULT    = 7,
      ECALL_USER_MODE           = 8,
      ECALL_SUPERVISOR_MODE     = 9,
      ECALL_MACHINE_MODE        = 11,
      INST_PAGE_FAULT           = 12,
      LOAD_PAGE_FAULT           = 13,
      STORE_AMO_PAGE_FAULT      = 15,
    };

    struct RevRegFile {
      RevRegFile(const RevRegFile&)            = default;  // Default copy constructor
      RevRegFile& operator=(const RevRegFile&) = default;  // Default assignment
      RevRegFile()                             = default;  // Default constructor
      ~RevRegFile()                            = default;  // Default destructor

      uint32_t RV32[_REV_NUM_REGS_];    ///< RevRegFile: RV32I register file
      uint64_t RV64[_REV_NUM_REGS_];    ///< RevRegFile: RV64I register file
      float SPF[_REV_NUM_REGS_];        ///< RevRegFile: RVxxF register file
      double DPF[_REV_NUM_REGS_];       ///< RevRegFile: RVxxD register file

      /* Supervisor Mode CSRs */
      uint64_t RV64_SSTATUS; // During ecall, previous priviledge mode is saved in this register (Incomplete)
      uint64_t RV64_SEPC;    // Holds address of instruction that caused the exception (ie. ECALL)
      uint64_t RV64_SCAUSE;  // Used to store cause of exception (ie. ECALL_USER_EXCEPTION)
      uint64_t RV64_STVAL;   // Used to store additional info about exception (ECALL does not use this and sets value to 0)
      uint64_t RV64_STVEC;   // Holds the base address of the exception handling routine (trap handler) that the processor jumps to when and exception occurs

      uint32_t RV32_SSTATUS;
      uint32_t RV32_SEPC;
      uint32_t RV32_SCAUSE;
      uint32_t RV32_STVAL;
      uint32_t RV32_STVEC;

      bool RV32_Scoreboard[_REV_NUM_REGS_]; ///< RevRegFile: Scoreboard for RV32I RF to manage pipeline hazard
      bool RV64_Scoreboard[_REV_NUM_REGS_]; ///< RevRegFile: Scoreboard for RV64I RF to manage pipeline hazard
      bool SPF_Scoreboard[_REV_NUM_REGS_];  ///< RevRegFile: Scoreboard for SPF RF to manage pipeline hazard
      bool DPF_Scoreboard[_REV_NUM_REGS_];  ///< RevRegFile: Scoreboard for DPF RF to manage pipeline hazard

      uint32_t RV32_PC;                 ///< RevRegFile: RV32 PC
      uint64_t RV64_PC;                 ///< RevRegFile: RV64 PC
      uint64_t FCSR;                    ///< RevRegFile: FCSR

      uint32_t cost;                    ///< RevRegFile: Cost of the instruction
      bool trigger;                     ///< RevRegFile: Has the instruction been triggered?
      unsigned Entry;                   ///< RevRegFile: Instruction entry

      /// AdvancePC: Advance the program counter a certain number of bytes
      template<typename T>
      void AdvancePC(RevFeature* F, T bytes) {
        if ( F->IsRV32() ) {
          RV32_PC += static_cast<int32_t>(bytes);
        }else{
          RV64_PC += static_cast<int64_t>(bytes);
        }
      }

      /// GetX: Get the specifed X register cast to a specific type
      template<typename T>
      T GetX(RevFeature* F, size_t rs) const {
        if( F->IsRV32() ){
          return static_cast<T>(rs ? RV32[rs] : 0);
        }else{
          return static_cast<T>(rs ? RV64[rs] : 0);
        }
      }

      /// SetX: Set the specifed X register to a specific value
      template<typename T>
      void SetX(RevFeature* F, size_t rd, T val) {
        if( F->IsRV32() ){
          RV32[rd] = rd ? static_cast<int32_t>(val) : 0;
        }else{
          RV64[rd] = rd ? static_cast<int64_t>(val) : 0;
        }
      }

      /// GetFP32: Get the 32-bit float value of a specific FP register
      float GetFP32(RevFeature* F, size_t rs) const {
        if( F->IsRV32D() || F->IsRV64D() ){
          uint64_t i64;
          memcpy(&i64, &DPF[rs], sizeof(i64));   // The FP64 register's value
          if (~i64 >> 32)                        // Check for boxed NaN
            return NAN;                          // Return NaN if it's not boxed
          auto i32 = static_cast<uint32_t>(i64); // For endian independence
          float fp32;
          memcpy(&fp32, &i32, sizeof(fp32));     // The bottom half of FP64
          return fp32;                           // Reinterpreted as FP32
        } else {
          return SPF[rs];                        // The FP32 register's value
        }
      }

      /// SetFP32: Set a specific FP register to a 32-bit float value
      void SetFP32(RevFeature* F, size_t rd, float value)
      {
        if( F->IsRV32D() || F->IsRV64D() ){
          uint32_t i32;
          memcpy(&i32, &value, sizeof(i32));                 // The FP32 value
          uint64_t i64 = uint64_t{i32} | ~uint64_t{0} << 32; // Boxed NaN value
          memcpy(&DPF[rd], &i64, sizeof(DPF[rd]));           // Store in FP64 register
        } else {
          SPF[rd] = value;                                   // Store in FP32 register
        }
      }

    };                        ///< RevProc: register file construct


    inline std::bitset<_REV_HART_COUNT_> HART_CTS; ///< RevProc: Thread is clear to start (proceed with decode)
    inline std::bitset<_REV_HART_COUNT_> HART_CTE; ///< RevProc: Thread is clear to execute (no register dependencides)

    enum RevInstF : int {    ///< Rev CPU Instruction Formats
      RVTypeUNKNOWN = 0,     ///< RevInstf: Unknown format
      RVTypeR       = 1,     ///< RevInstF: R-Type
      RVTypeI       = 2,     ///< RevInstF: I-Type
      RVTypeS       = 3,     ///< RevInstF: S-Type
      RVTypeU       = 4,     ///< RevInstF: U-Type
      RVTypeB       = 5,     ///< RevInstF: B-Type
      RVTypeJ       = 6,     ///< RevInstF: J-Type
      RVTypeR4      = 7,     ///< RevInstF: R4-Type for AMOs
      // -- Compressed Formats
      RVCTypeCR     = 10,    ///< RevInstF: Compressed CR-Type
      RVCTypeCI     = 11,    ///< RevInstF: Compressed CI-Type
      RVCTypeCSS    = 12,    ///< RevInstF: Compressed CSS-Type
      RVCTypeCIW    = 13,    ///< RevInstF: Compressed CIW-Type
      RVCTypeCL     = 14,    ///< RevInstF: Compressed CL-Type
      RVCTypeCS     = 15,    ///< RevInstF: Compressed CS-Type
      RVCTypeCA     = 16,    ///< RevInstF: Compressed CA-Type
      RVCTypeCB     = 17,    ///< RevInstF: Compressed CB-Type
      RVCTypeCJ     = 18     ///< RevInstF: Compressed CJ-Type
    };

    enum RevRegClass : int { ///< Rev CPU Register Classes
      RegUNKNOWN    = 0,     ///< RevRegClass: Unknown register file
      RegIMM        = 1,     ///< RevRegClass: Treat the reg class like an immediate: S-Format
      RegGPR        = 2,     ///< RevRegClass: GPR reg file
      RegCSR        = 3,     ///< RevRegClass: CSR reg file
      RegFLOAT      = 4      ///< RevRegClass: Float register file
    };

    enum RevImmFunc : int {  ///< Rev Immediate Values
      FUnk          = 0,     ///< RevRegClass: Imm12 is not used
      FImm          = 1,     ///< RevRegClass: Imm12 is an immediate
      FEnc          = 2,     ///< RevRegClass: Imm12 is an encoding value
      FVal          = 3      ///< RevRegClass: Imm12 is an incoming register value
    };

    /*! \struct RevInst
     *  \brief Rev decoded instruction
     *
     * Contains all the details required to execute
     * following a successful crack + decode
     *
     */
    struct RevInst {
      uint8_t opcode;       ///< RevInst: opcode
      uint8_t funct2;       ///< RevInst: compressed funct2 value
      uint8_t funct3;       ///< RevInst: funct3 value
      uint8_t funct4;       ///< RevInst: compressed funct4 value
      uint8_t funct6;       ///< RevInst: compressed funct6 value
      uint8_t funct7;       ///< RevInst: funct7 value
      uint8_t rd;           ///< RevInst: rd value
      uint8_t rs1;          ///< RevInst: rs1 value
      uint8_t rs2;          ///< RevInst: rs2 value
      uint8_t rs3;          ///< RevInst: rs3 value
      uint32_t imm;         ///< RevInst: immediate value
      uint8_t fmt;          ///< RevInst: floating point format
      uint8_t rm;           ///< RevInst: floating point rounding mode
      uint8_t aq;           ///< RevInst: aq field for atomic instructions
      uint8_t rl;           ///< RevInst: rl field for atomic instructions
      uint16_t offset;      ///< RevInst: compressed offset
      uint16_t jumpTarget;  ///< RevInst: compressed jumpTarget
      size_t instSize;      ///< RevInst: size of the instruction in bytes
      bool compressed;      ///< RevInst: determines if the instruction is compressed
      uint32_t cost;        ///< RevInst: the cost to execute this instruction, in clock cycles
      unsigned entry;       ///< RevInst: Where to find this instruction in the InstTables
      bool *hazard;         ///< RevInst: signals a load hazard

      ///< RevInst: Sign-extended immediate value
      constexpr int32_t ImmSignExt(size_t bits) const {
        auto tmp = imm & uint32_t{1} << (bits-1) ? imm | ~uint32_t{0} << (bits-1) : imm;
        // This needs to be signed so that it sign-extends when mixed with uint64_t
        return static_cast<int32_t>(tmp);
      }
    };

    static_assert(std::is_aggregate_v<RevInst>,
                  "RevInst must be an aggregate type (https://en.cppreference.com/w/cpp/language/aggregate_initialization)");

    /// RevInstEntry: Holds the compressed index to normal index mapping
    inline std::map<uint8_t,uint8_t> CRegMap =
    {
      {0b000,8},
      {0b001,9},
      {0b010,10},
      {0b011,11},
      {0b100,12},
      {0b101,13},
      {0b110,14},
      {0b111,15},
    };

    struct RevInstDefaults {
      uint8_t     opcode      = 0b00000000;
      uint32_t    cost        = 1;
      uint8_t     funct2      = 0b000;      // compressed only
      uint8_t     funct3      = 0b000;
      uint8_t     funct4      = 0b000;      // compressed only
      uint8_t     funct6      = 0b000;      // compressed only
      uint8_t     funct7      = 0b0000000;
      uint16_t    offset      = 0b0000000;  // compressed only
      uint16_t    jumpTarget  = 0b0000000;  // compressed only
      RevRegClass rdClass     = RegGPR;
      RevRegClass rs1Class    = RegGPR;
      RevRegClass rs2Class    = RegGPR;
      RevRegClass rs3Class    = RegUNKNOWN;
      uint16_t    imm12       = 0b000000000000;
      RevImmFunc  imm         = FUnk;
      RevInstF    format      = RVTypeR;
      bool        compressed  = false;
      uint8_t     fpcvtOp     = 0b00000;    // overloaded rs2 field for R-type FP instructions
    };

    /*! \struct RevInstEntry
     *  \brief Rev instruction entry
     *
     * Contains all the details required to decode and execute
     * a target instruction as well as its cost function
     *
     */
    typedef struct {
        // disassembly
        std::string mnemonic; ///< RevInstEntry: instruction mnemonic
        uint32_t cost;        ///< RevInstEntry: instruction code in cycles

        // storage
        uint8_t opcode;       ///< RevInstEntry: opcode
        uint8_t funct2;       ///< RevInstentry: compressed funct2 value
        uint8_t funct3;       ///< RevInstEntry: funct3 value
        uint8_t funct4;       ///< RevInstentry: compressed funct4 value
        uint8_t funct6;       ///< RevInstentry: compressed funct6 value
        uint8_t funct7;       ///< RevInstEntry: funct7 value
        uint16_t offset;      ///< RevInstEntry: compressed offset value
        uint16_t jumpTarget;  ///< RevInstEntry: compressed jump target value

        // register encodings
        RevRegClass rdClass;  ///< RevInstEntry: Rd register class
        RevRegClass rs1Class; ///< RevInstEntry: Rs1 register class
        RevRegClass rs2Class; ///< RevInstEntry: Rs2 register class
        RevRegClass rs3Class; ///< RevInstEntry: Rs3 register class

        uint16_t imm12;       ///< RevInstEntry: imm12 value

        RevImmFunc imm;       ///< RevInstEntry: does the imm12 exist?

        // formatting
        RevInstF format;      ///< RevInstEntry: instruction format

        /// RevInstEntry: Instruction implementation function
        bool (*func)(RevFeature *, RevRegFile *, RevMem *, RevInst);

        bool compressed;      ///< RevInstEntry: compressed instruction

        uint8_t fpcvtOp;   ///<RenInstEntry: Stores the overloaded rs2 field in R-type instructions
      } RevInstEntry;


    template <typename RevInstDefaultsPolicy>
    class RevInstEntryBuilder : public RevInstDefaultsPolicy{
      public:

      RevInstEntry InstEntry;

      RevInstEntryBuilder() : RevInstDefaultsPolicy() {
        //Set default values
        InstEntry.mnemonic  = std::string("nop");
        InstEntry.func      = NULL;
        InstEntry.opcode    = RevInstDefaultsPolicy::opcode;
        InstEntry.cost      = RevInstDefaultsPolicy::cost;
        InstEntry.funct2    = RevInstDefaultsPolicy::funct2;
        InstEntry.funct3    = RevInstDefaultsPolicy::funct3;
        InstEntry.funct4    = RevInstDefaultsPolicy::funct4;
        InstEntry.funct6    = RevInstDefaultsPolicy::funct6;
        InstEntry.funct7    = RevInstDefaultsPolicy::funct7;
        InstEntry.offset    = RevInstDefaultsPolicy::offset;
        InstEntry.jumpTarget= RevInstDefaultsPolicy::jumpTarget;
        InstEntry.rdClass   = RevInstDefaultsPolicy::rdClass;
        InstEntry.rs1Class  = RevInstDefaultsPolicy::rs1Class;
        InstEntry.rs2Class  = RevInstDefaultsPolicy::rs2Class;
        InstEntry.rs3Class  = RevInstDefaultsPolicy::rs3Class;
        InstEntry.imm12     = RevInstDefaultsPolicy::imm12;
        InstEntry.imm       = RevInstDefaultsPolicy::imm;
        InstEntry.format    = RevInstDefaultsPolicy::format;
        InstEntry.compressed= false;
        InstEntry.fpcvtOp  = RevInstDefaultsPolicy::fpcvtOp;
      }

      // Begin Set() functions to allow call chaining - all Set() must return *this
      RevInstEntryBuilder& SetMnemonic(std::string m)   { InstEntry.mnemonic = m;   return *this;};
      RevInstEntryBuilder& SetCost(uint32_t c)          { InstEntry.cost = c;       return *this;};
      RevInstEntryBuilder& SetOpcode(uint8_t op)        { InstEntry.opcode = op;    return *this;};
      RevInstEntryBuilder& SetFunct2(uint8_t f2)        { InstEntry.funct2 = f2;    return *this;};
      RevInstEntryBuilder& SetFunct3(uint8_t f3)        { InstEntry.funct3 = f3;    return *this;};
      RevInstEntryBuilder& SetFunct4(uint8_t f4)        { InstEntry.funct4 = f4;    return *this;};
      RevInstEntryBuilder& SetFunct6(uint8_t f6)        { InstEntry.funct6 = f6;    return *this;};
      RevInstEntryBuilder& SetFunct7(uint8_t f7)        { InstEntry.funct7 = f7;    return *this;};
      RevInstEntryBuilder& SetOffset(uint16_t off)      { InstEntry.offset = off;   return *this;};
      RevInstEntryBuilder& SetJumpTarget(uint16_t jt)   { InstEntry.jumpTarget = jt;return *this;};
      RevInstEntryBuilder& SetrdClass(RevRegClass rd)   { InstEntry.rdClass = rd;   return *this;};
      RevInstEntryBuilder& Setrs1Class(RevRegClass rs1) {InstEntry.rs1Class = rs1;  return *this;};
      RevInstEntryBuilder& Setrs2Class(RevRegClass rs2) {InstEntry.rs2Class = rs2;  return *this;};
      RevInstEntryBuilder& Setrs3Class(RevRegClass rs3) {InstEntry.rs3Class = rs3;  return *this;};
      RevInstEntryBuilder& Setimm12(uint16_t imm12)     {InstEntry.imm12 = imm12;   return *this;};
      RevInstEntryBuilder& Setimm(RevImmFunc imm)       {InstEntry.imm = imm;       return *this;};
      RevInstEntryBuilder& SetFormat(RevInstF format)   {InstEntry.format = format; return *this;};
      RevInstEntryBuilder& SetCompressed(bool c)        {InstEntry.compressed = c;  return *this;};
      RevInstEntryBuilder& SetfpcvtOp(uint8_t op)       {InstEntry.fpcvtOp = op;    return *this;};

      RevInstEntryBuilder& SetImplFunc(bool (*func)(RevFeature *,
                                                    RevRegFile *,
                                                    RevMem *,
                                                    RevInst)){
        InstEntry.func = func; return *this;};

    }; // class RevInstEntryBuilder;

    /// General template for converting between Floating Point and Integer.
    /// FP values outside the range of the target integer type are clipped
    /// at the integer type's numerical limits, whether signed or unsigned.
    template<typename FP, typename INT>
    static bool CvtFpToInt(RevFeature *F, RevRegFile *R, RevMem *M, RevInst Inst) {
      FP fp;
      if constexpr(std::is_same_v<FP, double>){
        fp = R->DPF[Inst.rs1];         // Read the double FP register directly
      }else{
        fp = R->GetFP32(F, Inst.rs1);  // Read the F or D register, unboxing if D
      }
      constexpr INT max = std::numeric_limits<INT>::max();
      constexpr INT min = std::numeric_limits<INT>::min();
      INT res = std::isnan(fp) || fp > FP(max) ? max : fp < FP(min) ? min : static_cast<INT>(fp);

      // Make final result signed so sign extension occurs when sizeof(INT) < XLEN
      R->SetX(F, Inst.rd, static_cast<std::make_signed_t<INT>>(res));

      R->AdvancePC(F, Inst.instSize);
      return true;
    }

    /// fclass: Return FP classification like the RISC-V fclass instruction
    // See: https://github.com/riscv/riscv-isa-sim/blob/master/softfloat/f32_classify.c
    // Because quiet and signaling NaNs are not distinguished by the C++ standard, an
    // additional argument has been added to disambiguate between quiet and signaling
    // NaNs. The argument will determine whether it's a quiet or signaling NaN.
    template<typename T>
    unsigned fclass(T val, bool quietNaN = true) {
      switch(std::fpclassify(val)){
      case FP_INFINITE:
        return std::signbit(val) ? 1 : 1 << 7;
      case FP_NAN:
        return quietNaN ? 1 << 9 : 1 << 8;
      case FP_NORMAL:
        return std::signbit(val) ? 1 << 1 : 1 << 6;
      case FP_SUBNORMAL:
        return std::signbit(val) ? 1 << 2 : 1 << 5;
      case FP_ZERO:
        return std::signbit(val) ? 1 << 3 : 1 << 4;
      default:
        return 0;
      }
    }
  } // namespace RevCPU
} // namespace SST

#endif
