#include "maat/arch.hpp"
#include "maat/engine.hpp"
#include "maat/exception.hpp"
#include "maat/expression.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

using std::string;

namespace test{
    namespace archARM64{
        
        using namespace maat;
        
        unsigned int _assert(bool val, const std::string& msg)
        {
            if( !val){
                std::cout << "\nFail: " << msg << std::endl << std::flush; 
                throw test_exception();
            }
            return 1;
        }
        
        unsigned int _assert_bignum_eq(
            const Value& var,
            std::string expected_value,
            std::string error_msg
        )
        {
            const Number& number = var.as_number();
            std::stringstream ss;
            ss << number;
            if (ss.str() != expected_value)
            {
                std::cout << "\nFail: _assert_bignum_eq: " << ss.str() << " is not " << expected_value << std::endl;
                std::cout << "\nFail: " << error_msg << std::endl;
                throw test_exception(); 
            }
            return 1; 
        }
         
        unsigned int reg_translation()
        {
            unsigned int nb = 0;
            reg_t reg;
            ARM64::ArchARM64 arch = ARM64::ArchARM64();
            for (reg = 0; reg < ARM64::NB_REGS; reg++)
            {
                nb += _assert( arch.reg_num(arch.reg_name(reg)) == reg , "ArchARM64: translation reg_num <-> reg_name failed");
            }
            nb += _assert(arch.sp() == ARM64::SP, "ArchARM64: sp() should return SP");
            nb += _assert(arch.pc() == ARM64::PC, "ArchARM64: pc() should return PC");
            nb += _assert(arch.tsc() == ARM64::CNTPCT_EL0, "ArchARM64: tsc() should return CNTPCT_EL0");
            return nb;
        }
        
        unsigned int register_aliasing()
        {
            unsigned int nb = 0;
            MaatEngine engine(Arch::Type::ARM64);
            
            // Test zero register behavior
            engine.cpu.ctx().set(ARM64::ZR, exprcst(64, 0x12345678));
            nb += _assert(engine.cpu.ctx().get(ARM64::ZR).as_uint(*engine.vars) == 0, "ARM64: ZR should always read as zero");
            
            return nb;
        }
        
        unsigned int test_flags_registers()
        {
            unsigned int nb = 0;
            MaatEngine engine(Arch::Type::ARM64);
            ARM64::ArchARM64 arch = ARM64::ArchARM64();
            
            // Test that NZCV register is defined and has correct size
            nb += _assert(arch.reg_size(ARM64::NZCV) == 32, "ARM64: NZCV register should be 32-bit");
            nb += _assert(arch.reg_num("nzcv") == ARM64::NZCV, "ARM64: NZCV register should be recognized by name");
            
            // Test individual flag registers can be manipulated
            engine.cpu.ctx().set(ARM64::NF, exprcst(8, 1));
            nb += _assert(engine.cpu.ctx().get(ARM64::NF).as_uint(*engine.vars) == 1, "ARM64: NF flag read/write");
            
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 1));
            nb += _assert(engine.cpu.ctx().get(ARM64::ZF).as_uint(*engine.vars) == 1, "ARM64: ZF flag read/write");
            
            engine.cpu.ctx().set(ARM64::CF, exprcst(8, 0));
            nb += _assert(engine.cpu.ctx().get(ARM64::CF).as_uint(*engine.vars) == 0, "ARM64: CF flag read/write");
            
            engine.cpu.ctx().set(ARM64::VF, exprcst(8, 1));
            nb += _assert(engine.cpu.ctx().get(ARM64::VF).as_uint(*engine.vars) == 1, "ARM64: VF flag read/write");
            
            return nb;
        }
        
        unsigned int test_special_registers()
        {
            unsigned int nb = 0;
            ARM64::ArchARM64 arch = ARM64::ArchARM64();
            
            // Test that architecture recognizes X29 and X30 registers by name
            nb += _assert(arch.reg_num("x29") == ARM64::X29, "ARM64: X29 register should be recognized by name");
            nb += _assert(arch.reg_num("x30") == ARM64::X30, "ARM64: X30 register should be recognized by name");
            
            // Test that architecture recognizes FP and LR aliases
            nb += _assert(arch.reg_num("fp") == ARM64::FP, "ARM64: FP alias should be recognized");
            nb += _assert(arch.reg_num("lr") == ARM64::LR, "ARM64: LR alias should be recognized");
            
            // Test register sizes for special registers
            nb += _assert(arch.reg_size(ARM64::X29) == 64, "ARM64: X29 frame pointer should be 64-bit");
            nb += _assert(arch.reg_size(ARM64::X30) == 64, "ARM64: X30 link register should be 64-bit");
            
            // Test that FP and LR aliases are correctly defined (compile-time constants)
            nb += _assert(ARM64::FP == ARM64::X29, "ARM64: FP should be alias for X29");
            nb += _assert(ARM64::LR == ARM64::X30, "ARM64: LR should be alias for X30");
            
            // Test that register names can be resolved (don't care about specific name due to aliases)
            std::string x29_name = arch.reg_name(ARM64::X29);
            std::string x30_name = arch.reg_name(ARM64::X30);
            nb += _assert(!x29_name.empty(), "ARM64: X29 should have a name");
            nb += _assert(!x30_name.empty(), "ARM64: X30 should have a name");
            
            return nb;
        }

        unsigned int register_sizes()
        {
            unsigned int nb = 0;
            ARM64::ArchARM64 arch = ARM64::ArchARM64();
            
            // Test 64-bit X registers
            nb += _assert(arch.reg_size(ARM64::X0) == 64, "ARM64: X0 size should be 64 bits");
            nb += _assert(arch.reg_size(ARM64::X30) == 64, "ARM64: X30 size should be 64 bits");
            nb += _assert(arch.reg_size(ARM64::SP) == 64, "ARM64: SP size should be 64 bits");
            nb += _assert(arch.reg_size(ARM64::PC) == 64, "ARM64: PC size should be 64 bits");
            
            
            // Test status flags
            nb += _assert(arch.reg_size(ARM64::NF) == 8, "ARM64: NF flag size should be 8 bits");
            nb += _assert(arch.reg_size(ARM64::ZF) == 8, "ARM64: ZF flag size should be 8 bits");
            nb += _assert(arch.reg_size(ARM64::CF) == 8, "ARM64: CF flag size should be 8 bits");
            nb += _assert(arch.reg_size(ARM64::VF) == 8, "ARM64: VF flag size should be 8 bits");
            
            // Test NZCV composite register
            nb += _assert(arch.reg_size(ARM64::NZCV) == 32, "ARM64: NZCV size should be 32 bits");
            
            return nb;
        }
        
        unsigned int disass_add(MaatEngine& engine)
        {
            unsigned int nb = 0;
            std::string code;
            
            // Test ADD X0, X1, X2 (64-bit add)
            code = std::string("\x20\x00\x02\x8B", 4); // add x0, x1, x2
            engine.mem->write_buffer(0x1000, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1004, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // brk #0
            
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x1000));
            engine.cpu.ctx().set(ARM64::X2, exprcst(64, 0x2000));
            engine.run_from(0x1000, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x3000,
                          "ARM64: failed to execute ADD X0, X1, X2");
            
            // Test 32-bit ADD instruction
            code = std::string("\x20\x00\x02\x0B", 4); // add w0, w1, w2
            engine.mem->write_buffer(0x1008, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x100C, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // brk #0
            
            // Set X1 and X2 with high bits set to verify W instruction clears them
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0xFFFFFFFF80000000ULL));
            engine.cpu.ctx().set(ARM64::X2, exprcst(64, 0xFFFFFFFF80000000ULL));
            engine.run_from(0x1008, 1);
            
            // W instruction should have cleared upper 32 bits of X0 result
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0,
                          "ARM64: failed to execute ADD W0, W1, W2 (32-bit arithmetic)");
                          
            return nb;
        }
        
        unsigned int disass_mov(MaatEngine& engine)
        {
            unsigned int nb = 0;
            std::string code;
            
            // Test MOV X0, X1
            code = std::string("\xE0\x03\x01\xAA", 4); // mov x0, x1
            engine.mem->write_buffer(0x1010, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1014, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // brk #0
            
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x123456789ABCDEF0ULL));
            engine.run_from(0x1010, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x123456789ABCDEF0ULL,
                          "ARM64: failed to execute MOV X0, X1");
            
            // Test MOV X0, #immediate
            code = std::string("\x40\x02\x80\xD2", 4); // mov x0, #0x12
            engine.mem->write_buffer(0x1018, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x101C, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // brk #0
            
            engine.run_from(0x1018, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x12,
                          "ARM64: failed to execute MOV X0, #0x12");
                          
            return nb;
        }
        
        unsigned int disass_ldr(MaatEngine& engine)
        {
            unsigned int nb = 0;
            std::string code;
            
            // Test LDR X0, [X1]
            code = std::string("\x20\x00\x40\xF9", 4); // ldr x0, [x1]
            engine.mem->write_buffer(0x1020, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1024, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // brk #0
            
            // Set up memory with test data
            engine.mem->write(0x2000, exprcst(64, 0xDEADBEEFCAFEBABEULL));
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x2000));
            
            engine.run_from(0x1020, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0xDEADBEEFCAFEBABEULL,
                          "ARM64: failed to execute LDR X0, [X1]");
                          
            return nb;
        }
        
        unsigned int disass_str(MaatEngine& engine)
        {
            unsigned int nb = 0;
            std::string code;
            
            // Test STR X0, [X1]
            code = std::string("\x20\x00\x00\xF9", 4); // str x0, [x1]
            engine.mem->write_buffer(0x1028, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x102C, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // brk #0
            
            engine.cpu.ctx().set(ARM64::X0, exprcst(64, 0x123456789ABCDEF0ULL));
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x3000));
            
            engine.run_from(0x1028, 1);
            
            nb += _assert(engine.mem->read(0x3000, 8).as_uint(*engine.vars) == 0x123456789ABCDEF0ULL,
                          "ARM64: failed to execute STR X0, [X1]");
                          
            return nb;
        }
        
        unsigned int disass_cmp(MaatEngine& engine)
        {
            unsigned int nb = 0;
            std::string code;
            
            // Test CMP X0, X1
            code = std::string("\x1F\x00\x01\xEB", 4); // cmp x0, x1
            engine.mem->write_buffer(0x1030, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1034, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // brk #0
            
            // Test equal values
            engine.cpu.ctx().set(ARM64::X0, exprcst(64, 0x1000));
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x1000));
            engine.run_from(0x1030, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::ZF).as_uint(*engine.vars) == 1,
                          "ARM64: CMP should set ZF for equal values");
            nb += _assert(engine.cpu.ctx().get(ARM64::CF).as_uint(*engine.vars) == 1,
                          "ARM64: CMP should set CF for equal values");
                          
            // Test X0 < X1
            engine.cpu.ctx().set(ARM64::X0, exprcst(64, 0x500));
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x1000));
            engine.run_from(0x1030, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::ZF).as_uint(*engine.vars) == 0,
                          "ARM64: CMP should clear ZF for unequal values");
            nb += _assert(engine.cpu.ctx().get(ARM64::CF).as_uint(*engine.vars) == 0,
                          "ARM64: CMP should clear CF when X0 < X1");
                          
            return nb;
        }
        
        unsigned int disass_sub(MaatEngine& engine)
        {
            unsigned int nb = 0;
            std::string code;
            
            // Test SUB X0, X1, X2 (64-bit register subtract)
            code = std::string("\x20\x00\x02\xCB", 4); // SUB X0, X1, X2
            engine.mem->write_buffer(0x1100, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1104, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x3000));
            engine.cpu.ctx().set(ARM64::X2, exprcst(64, 0x1000));
            
            engine.run_from(0x1100, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x2000,
                          "ARM64: failed to execute SUB X0, X1, X2");
                          
            // Test SUB W0, W1, W2 (32-bit with upper bit clearing)
            code = std::string("\x20\x00\x02\x4B", 4); // SUB W0, W1, W2
            engine.mem->write_buffer(0x1110, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1114, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0xFFFFFFFF50000000));
            engine.cpu.ctx().set(ARM64::X2, exprcst(64, 0x10000000));
            
            engine.run_from(0x1110, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x40000000,
                          "ARM64: SUB W0 failed to clear upper 32 bits");
                          
            // Test SUB X0, X1, #immediate
            code = std::string("\x20\x40\x00\xD1", 4); // SUB X0, X1, #0x10
            engine.mem->write_buffer(0x1120, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1124, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x1000));
            
            engine.run_from(0x1120, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0xFF0,
                          "ARM64: failed to execute SUB X0, X1, #0x10");
            
            return nb;
        }
        
        unsigned int disass_mul(MaatEngine& engine)
        {
            unsigned int nb = 0;
            std::string code;
            
            // Test MUL X0, X1, X2 (64-bit multiply)
            code = std::string("\x20\x7C\x02\x9B", 4); // MUL X0, X1, X2
            engine.mem->write_buffer(0x1200, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1204, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x123));
            engine.cpu.ctx().set(ARM64::X2, exprcst(64, 0x456));
            
            engine.run_from(0x1200, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == (0x123 * 0x456),
                          "ARM64: failed to execute MUL X0, X1, X2");
                          
            // Test MUL W0, W1, W2 (32-bit multiply)
            code = std::string("\x20\x7C\x02\x1B", 4); // MUL W0, W1, W2
            engine.mem->write_buffer(0x1210, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1214, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0xFFFFFFFF00000100));
            engine.cpu.ctx().set(ARM64::X2, exprcst(64, 0xFFFFFFFF00000200));
            
            engine.run_from(0x1210, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x20000,
                          "ARM64: MUL W0 failed to multiply and clear upper bits");
                          
            // Test multiply by 0
            engine.cpu.ctx().set(ARM64::X2, exprcst(64, 0));
            engine.run_from(0x1200, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0,
                          "ARM64: MUL failed with zero operand");
            
            return nb;
        }
        
        unsigned int disass_div(MaatEngine& engine)
        {
            unsigned int nb = 0;
            std::string code;
            
            // Test UDIV X0, X1, X2 (unsigned divide)
            code = std::string("\x20\x08\xC2\x9A", 4); // UDIV X0, X1, X2
            engine.mem->write_buffer(0x1300, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1304, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x1000));
            engine.cpu.ctx().set(ARM64::X2, exprcst(64, 0x10));
            
            engine.run_from(0x1300, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x100,
                          "ARM64: failed to execute UDIV X0, X1, X2");
                          
            // Test SDIV X0, X1, X2 (signed divide)
            code = std::string("\x20\x0C\xC2\x9A", 4); // SDIV X0, X1, X2
            engine.mem->write_buffer(0x1310, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1314, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x2000));
            engine.cpu.ctx().set(ARM64::X2, exprcst(64, 0x40));
            
            engine.run_from(0x1310, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x80,
                          "ARM64: failed to execute SDIV X0, X1, X2");
                          
            // Test UDIV W0, W1, W2 (32-bit unsigned)
            code = std::string("\x20\x08\xC2\x1A", 4); // UDIV W0, W1, W2
            engine.mem->write_buffer(0x1320, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1324, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0xFFFFFFFF80000000));
            engine.cpu.ctx().set(ARM64::X2, exprcst(64, 0x2));
            
            engine.run_from(0x1320, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x40000000,
                          "ARM64: UDIV W0 failed to divide and clear upper bits");
                          
            // Test divide by 1
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x1000));
            engine.cpu.ctx().set(ARM64::X2, exprcst(64, 1));
            engine.run_from(0x1300, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x1000,
                          "ARM64: UDIV failed with divisor 1");
            
            return nb;
        }
        
        unsigned int disass_csel(MaatEngine& engine)
        {
            unsigned int nb = 0;
            std::string code;
            
            // Test CSEL X0, X1, X2, EQ with ZF=1 (should select X1)
            code = std::string("\x20\x00\x82\x9A", 4); // CSEL X0, X1, X2, EQ
            engine.mem->write_buffer(0x1800, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1804, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x1111111111111111));
            engine.cpu.ctx().set(ARM64::X2, exprcst(64, 0x2222222222222222));
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 1)); // Set ZF for EQ condition
            
            engine.run_from(0x1800, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x1111111111111111,
                          "ARM64: CSEL failed to select X1 when condition true");
            
            // Test with ZF=0 (should select X2)
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 0)); // Clear ZF
            engine.run_from(0x1800, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x2222222222222222,
                          "ARM64: CSEL failed to select X2 when condition false");
                          
            // Test CSEL W0, W1, W2, NE (32-bit)
            code = std::string("\x20\x10\x82\x1A", 4); // CSEL W0, W1, W2, NE
            engine.mem->write_buffer(0x1810, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1814, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0xFFFFFFFF33333333));
            engine.cpu.ctx().set(ARM64::X2, exprcst(64, 0xFFFFFFFF44444444));
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 0)); // Clear ZF for NE condition
            
            engine.run_from(0x1810, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x33333333,
                          "ARM64: CSEL W0 failed to select W1 and clear upper bits");
                          
            // Test CSINC X0, X1, X2, EQ (should increment X2 when condition false)
            code = std::string("\x20\x04\x82\x9A", 4); // CSINC X0, X1, X2, EQ
            engine.mem->write_buffer(0x1820, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1824, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x1000));
            engine.cpu.ctx().set(ARM64::X2, exprcst(64, 0x2000));
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 0)); // Clear ZF (condition false)
            
            engine.run_from(0x1820, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x2001,
                          "ARM64: CSINC failed to increment X2 when condition false");
            
            return nb;
        }
        
        unsigned int disass_cset_cinc(MaatEngine& engine)
        {
            unsigned int nb = 0;
            std::string code;
            
            // Test CSET X0, EQ with ZF=1 (should set to 1)
            code = std::string("\xE0\x17\x9F\x9A", 4); // CSET X0, EQ (actually CSINC X0, XZR, XZR, NE)
            engine.mem->write_buffer(0x1900, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1904, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 1)); // Set ZF for EQ condition
            
            engine.run_from(0x1900, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 1,
                          "ARM64: CSET failed to set 1 when condition true");
                          
            // Test CSET X0, EQ with ZF=0 (should set to 0)
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 0)); // Clear ZF
            engine.run_from(0x1900, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0,
                          "ARM64: CSET failed to set 0 when condition false");
                          
            // Test CSET W0, NE (32-bit)
            code = std::string("\xE0\x07\x9F\x1A", 4); // CSET W0, NE (actually CSINC W0, WZR, WZR, EQ)
            engine.mem->write_buffer(0x1910, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1914, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.cpu.ctx().set(ARM64::X0, exprcst(64, 0xFFFFFFFFFFFFFFFF)); // Set all bits
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 0)); // Clear ZF for NE condition
            
            engine.run_from(0x1910, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 1,
                          "ARM64: CSET W0 failed to clear upper bits and set to 1");
                          
            // Test CINC X0, X1, EQ (should increment X1 when condition true)
            code = std::string("\x20\x14\x81\x9A", 4); // CINC X0, X1, EQ (actually CSINC X0, X1, X1, NE)
            engine.mem->write_buffer(0x1920, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1924, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x5000));
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 1)); // Set ZF for EQ condition
            
            engine.run_from(0x1920, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x5001,
                          "ARM64: CINC failed to increment when condition true");
                          
            // Test CINC with condition false (should not increment)
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 0)); // Clear ZF
            engine.run_from(0x1920, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0x5000,
                          "ARM64: CINC failed to keep original value when condition false");
            
            return nb;
        }
        
        unsigned int disass_branch(MaatEngine& engine)
        {
            unsigned int nb = 0;
            std::string code;
            
            // Test B.EQ with ZF=1 (should branch to target at +8)
            code = std::string("\x40\x00\x00\x54", 4); // B.EQ #8
            engine.mem->write_buffer(0x1400, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1408, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0 at branch target
            engine.mem->write_buffer(0x1404, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0 at fall-through
            
            // Branch taken case
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 1)); // Set ZF for EQ
            engine.run_from(0x1400, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::PC).as_uint(*engine.vars) == 0x1408,
                          "ARM64: B.EQ failed to branch when ZF set");
                          
            // Branch not taken case
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 0)); // Clear ZF
            engine.run_from(0x1400, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::PC).as_uint(*engine.vars) == 0x1404,
                          "ARM64: B.EQ incorrectly branched when ZF clear");
                          
            // Test B.NE with ZF=0 (should branch)
            code = std::string("\x41\x00\x00\x54", 4); // B.NE #8
            engine.mem->write_buffer(0x1410, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1418, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0 at target
            engine.mem->write_buffer(0x1414, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0 at fall-through
            
            // Branch taken case
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 0)); // Clear ZF for NE
            engine.run_from(0x1410, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::PC).as_uint(*engine.vars) == 0x1418,
                          "ARM64: B.NE failed to branch when ZF clear");
                          
            return nb;
        }
        
        unsigned int disass_cbz_cbnz(MaatEngine& engine)
        {
            unsigned int nb = 0;
            std::string code;
            
            // Test CBZ X0, target with X0=0 (should branch)
            code = std::string("\x40\x00\x00\xB4", 4); // CBZ X0, #8
            engine.mem->write_buffer(0x1500, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1508, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0 at target
            engine.mem->write_buffer(0x1504, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0 at fall-through
            
            // Branch taken case (X0 = 0)
            engine.cpu.ctx().set(ARM64::X0, exprcst(64, 0));
            engine.run_from(0x1500, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::PC).as_uint(*engine.vars) == 0x1508,
                          "ARM64: CBZ failed to branch when X0 is zero");
                          
            // Branch not taken case (X0 != 0)
            engine.cpu.ctx().set(ARM64::X0, exprcst(64, 0x1234));
            engine.run_from(0x1500, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::PC).as_uint(*engine.vars) == 0x1504,
                          "ARM64: CBZ incorrectly branched when X0 is non-zero");
                          
            // Test CBNZ X0, target with X0!=0 (should branch)
            code = std::string("\x40\x00\x00\xB5", 4); // CBNZ X0, #8
            engine.mem->write_buffer(0x1510, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1518, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0 at target
            engine.mem->write_buffer(0x1514, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0 at fall-through
            
            // Branch taken case (X0 != 0)
            engine.cpu.ctx().set(ARM64::X0, exprcst(64, 0x1234));
            engine.run_from(0x1510, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::PC).as_uint(*engine.vars) == 0x1518,
                          "ARM64: CBNZ failed to branch when X0 is non-zero");
                          
            return nb;
        }
        
        unsigned int disass_bitwise(MaatEngine& engine)
        {
            unsigned int nb = 0;
            std::string code;
            
            // Test AND X0, X1, X2
            code = std::string("\x20\x00\x02\x8A", 4); // AND X0, X1, X2
            engine.mem->write_buffer(0x1600, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1604, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0xFF00FF00FF00FF00));
            engine.cpu.ctx().set(ARM64::X2, exprcst(64, 0x00FF00FF00FF00FF));
            
            engine.run_from(0x1600, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0,
                          "ARM64: AND failed to execute correctly");
                          
            // Test ORR X0, X1, X2
            code = std::string("\x20\x00\x02\xAA", 4); // ORR X0, X1, X2
            engine.mem->write_buffer(0x1610, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1614, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.run_from(0x1610, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0xFFFFFFFFFFFFFFFF,
                          "ARM64: ORR failed to execute correctly");
                          
            // Test EOR X0, X1, X2
            code = std::string("\x20\x00\x02\xCA", 4); // EOR X0, X1, X2
            engine.mem->write_buffer(0x1620, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1624, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // BRK #0
            
            engine.run_from(0x1620, 1);
            
            nb += _assert(engine.cpu.ctx().get(ARM64::X0).as_uint(*engine.vars) == 0xFFFFFFFFFFFFFFFF,
                          "ARM64: EOR failed to execute correctly");
                          
            return nb;
        }
        
        unsigned int test_nzcv_composite()
        {
            unsigned int nb = 0;
            MaatEngine engine(Arch::Type::ARM64);
            
            // Test 1: Setting individual flags updates NZCV composite
            engine.cpu.ctx().set(ARM64::NF, exprcst(8, 1));  // N=1
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 1));  // Z=1
            engine.cpu.ctx().set(ARM64::CF, exprcst(8, 0));  // C=0
            engine.cpu.ctx().set(ARM64::VF, exprcst(8, 1));  // V=1
            
            // NZCV should be 0xD0000000 (binary: 1101 in bits 31-28, N=1,Z=1,C=0,V=1)
            nb += _assert(engine.cpu.ctx().get(ARM64::NZCV).as_uint(*engine.vars) == 0xD0000000, 
                          "NZCV composite: packing flags N=1,Z=1,C=0,V=1");
            
            // Test 2: Setting NZCV updates individual flags
            engine.cpu.ctx().set(ARM64::NZCV, exprcst(32, 0x60000000));  // Binary: 0110 (N=0,Z=1,C=1,V=0)
            nb += _assert(engine.cpu.ctx().get(ARM64::NF).as_uint(*engine.vars) == 0, "NZCV unpack: N flag = 0");
            nb += _assert(engine.cpu.ctx().get(ARM64::ZF).as_uint(*engine.vars) == 1, "NZCV unpack: Z flag = 1");
            nb += _assert(engine.cpu.ctx().get(ARM64::CF).as_uint(*engine.vars) == 1, "NZCV unpack: C flag = 1");
            nb += _assert(engine.cpu.ctx().get(ARM64::VF).as_uint(*engine.vars) == 0, "NZCV unpack: V flag = 0");
            
            // Test 3: All flags set
            engine.cpu.ctx().set(ARM64::NF, exprcst(8, 1));
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 1));
            engine.cpu.ctx().set(ARM64::CF, exprcst(8, 1));
            engine.cpu.ctx().set(ARM64::VF, exprcst(8, 1));
            nb += _assert(engine.cpu.ctx().get(ARM64::NZCV).as_uint(*engine.vars) == 0xF0000000, 
                          "NZCV composite: all flags set (0xF0000000)");
            
            // Test 4: All flags clear
            engine.cpu.ctx().set(ARM64::NZCV, exprcst(32, 0x00000000));
            nb += _assert(engine.cpu.ctx().get(ARM64::NF).as_uint(*engine.vars) == 0, "NZCV unpack: all clear N");
            nb += _assert(engine.cpu.ctx().get(ARM64::ZF).as_uint(*engine.vars) == 0, "NZCV unpack: all clear Z");
            nb += _assert(engine.cpu.ctx().get(ARM64::CF).as_uint(*engine.vars) == 0, "NZCV unpack: all clear C");
            nb += _assert(engine.cpu.ctx().get(ARM64::VF).as_uint(*engine.vars) == 0, "NZCV unpack: all clear V");
            
            // Test 5: Different patterns - 0xB0000000 = binary 1011 (N=1,Z=0,C=1,V=1) 
            engine.cpu.ctx().set(ARM64::NZCV, exprcst(32, 0xB0000000));  // Binary: 1011 (N=1,Z=0,C=1,V=1)
            nb += _assert(engine.cpu.ctx().get(ARM64::NF).as_uint(*engine.vars) == 1, "NZCV pattern 1011: N=1");
            nb += _assert(engine.cpu.ctx().get(ARM64::ZF).as_uint(*engine.vars) == 0, "NZCV pattern 1011: Z=0");
            nb += _assert(engine.cpu.ctx().get(ARM64::CF).as_uint(*engine.vars) == 1, "NZCV pattern 1011: C=1");
            nb += _assert(engine.cpu.ctx().get(ARM64::VF).as_uint(*engine.vars) == 1, "NZCV pattern 1011: V=1");
            
            // Test 6: Another pattern - 0x90000000 = binary 1001 (N=1,Z=0,C=0,V=1)
            engine.cpu.ctx().set(ARM64::NZCV, exprcst(32, 0x90000000));  // Binary: 1001 (N=1,Z=0,C=0,V=1)
            nb += _assert(engine.cpu.ctx().get(ARM64::NF).as_uint(*engine.vars) == 1, "NZCV pattern 1001: N=1");
            nb += _assert(engine.cpu.ctx().get(ARM64::ZF).as_uint(*engine.vars) == 0, "NZCV pattern 1001: Z=0");
            nb += _assert(engine.cpu.ctx().get(ARM64::CF).as_uint(*engine.vars) == 0, "NZCV pattern 1001: C=0");
            nb += _assert(engine.cpu.ctx().get(ARM64::VF).as_uint(*engine.vars) == 1, "NZCV pattern 1001: V=1");
            
            // Test 7: Lower bits should be ignored when setting NZCV
            engine.cpu.ctx().set(ARM64::NZCV, exprcst(32, 0xFFFFFFFF));  // All bits set
            // Should only affect bits 31-28, so equivalent to 0xF0000000
            nb += _assert(engine.cpu.ctx().get(ARM64::NF).as_uint(*engine.vars) == 1, "NZCV with lower bits: N");
            nb += _assert(engine.cpu.ctx().get(ARM64::ZF).as_uint(*engine.vars) == 1, "NZCV with lower bits: Z");
            nb += _assert(engine.cpu.ctx().get(ARM64::CF).as_uint(*engine.vars) == 1, "NZCV with lower bits: C");
            nb += _assert(engine.cpu.ctx().get(ARM64::VF).as_uint(*engine.vars) == 1, "NZCV with lower bits: V");
            
            // Test 8: Verify NZCV also reads correctly when set through individual flags after lower bit test
            engine.cpu.ctx().set(ARM64::NF, exprcst(8, 0));  // N=0
            engine.cpu.ctx().set(ARM64::ZF, exprcst(8, 1));  // Z=1
            engine.cpu.ctx().set(ARM64::CF, exprcst(8, 1));  // C=1
            engine.cpu.ctx().set(ARM64::VF, exprcst(8, 0));  // V=0
            // Should be 0x60000000 = binary 0110 (N=0,Z=1,C=1,V=0)
            nb += _assert(engine.cpu.ctx().get(ARM64::NZCV).as_uint(*engine.vars) == 0x60000000, 
                          "NZCV composite: final packing test N=0,Z=1,C=1,V=0");
            
            return nb;
        }
        
        unsigned int test_simd_registers()
        {
            unsigned int nb = 0;
            ARM64::ArchARM64 arch = ARM64::ArchARM64();
            MaatEngine engine(Arch::Type::ARM64);
            
            nb += _assert(arch.reg_size(ARM64::Z0) == 256, "ArchARM64: Z0 size should be 256 bits");
            nb += _assert(arch.reg_size(ARM64::Z31) == 256, "ArchARM64: Z31 size should be 256 bits");
            
            engine.cpu.ctx().set(ARM64::Z0, exprcst(256, "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"));
            nb += _assert_bignum_eq(
                engine.cpu.ctx().get(ARM64::Z0),
                "0x123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef",
                "ArchARM64: Z0 register read/write"
            );
            
            engine.cpu.ctx().set(ARM64::Z15, exprcst(256, "fedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210"));
            nb += _assert_bignum_eq(
                engine.cpu.ctx().get(ARM64::Z15),
                "0xfedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210",
                "ArchARM64: Z15 register read/write"
            );
            
            return nb;
        }
        
        unsigned int disass_fmov(MaatEngine& engine)
        {
            unsigned int nb = 0;
            std::string code;
            
            code = std::string("\x20\x40\x60\x1E", 4); // fmov d0, d1
            engine.mem->write_buffer(0x1600, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1604, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // brk #0
            
            engine.cpu.ctx().set(ARM64::Z1, exprcst(256, "1122334455667788aabbccddeeff00111122334455667788aabbccddeeff0011"));
            engine.run_from(0x1600, 1);
            
            Value v0_result = engine.cpu.ctx().get(ARM64::Z0);
            nb += _assert(v0_result.as_expr()->size == 256, "ArchARM64: FMOV D0, D1 execution test");
            
            return nb;
        }
        
        unsigned int disass_ldr_q(MaatEngine& engine)
        {
            unsigned int nb = 0;
            std::string code;
            
            code = std::string("\x20\x00\xC0\x3D", 4); // ldr q0, [x1]
            engine.mem->write_buffer(0x1700, (uint8_t*)code.c_str(), 4);
            engine.mem->write_buffer(0x1704, (uint8_t*)std::string("\x00\x00\x20\xD4", 4).c_str(), 4); // brk #0
            
            engine.mem->write(0x4000, exprcst(128, "123456789abcdef0deadbeefcafebabe"));
            engine.cpu.ctx().set(ARM64::X1, exprcst(64, 0x4000));
            
            engine.run_from(0x1700, 1);
            
            nb += _assert_bignum_eq(
                extract(engine.cpu.ctx().get(ARM64::Z0).as_expr(), 127, 0),
                "0x123456789abcdef00000000000000000",
                "ArchARM64: LDR Q0, [X1] execution test"
            );
            
            return nb;
        }
        
    } // namespace archARM64
} // namespace test

using namespace test::archARM64; 

void test_archARM64()
{
    unsigned int total = 0;
    std::string green = "\033[1;32m";
    std::string def = "\033[0m";
    std::string bold = "\033[1m";
    
    // Start testing
    std::cout << bold << "[" << green << "+" 
         << def << bold << "]" << def << std::left << std::setw(34)
         << " Testing arch ARM64 support... " << std::flush;

    MaatEngine engine(Arch::Type::ARM64);
    engine.mem->map(0x1000, 0x2000);    // Code section
    engine.mem->map(0x2000, 0x3000);    // Data section for LDR tests
    engine.mem->map(0x3000, 0x4000);    // Data section for STR tests
    engine.mem->map(0x4000, 0x5000);    // Data section

    total += reg_translation();
    total += register_aliasing();
    total += test_flags_registers();
    total += test_special_registers();
    total += register_sizes();
    total += test_nzcv_composite();
    total += test_simd_registers();
    total += disass_fmov(engine);
    total += disass_ldr_q(engine);
    total += disass_add(engine);
    total += disass_mov(engine);
    total += disass_ldr(engine);
    total += disass_str(engine);
    total += disass_cmp(engine);
    total += disass_sub(engine);
    total += disass_mul(engine);
    total += disass_div(engine);
    total += disass_csel(engine);
    total += disass_cset_cinc(engine);
    total += disass_branch(engine);
    total += disass_cbz_cbnz(engine);
    total += disass_bitwise(engine);
    
    std::cout << "\t" << total << "/" << total << green << "\t\tOK" << def << std::endl;
}