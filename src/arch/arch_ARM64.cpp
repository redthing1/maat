#include "maat/arch.hpp"
#include "maat/exception.hpp"

namespace maat
{

namespace ARM64
{
    ArchARM64::ArchARM64(): Arch(Arch::Type::ARM64, 64, ARM64::NB_REGS)
    {
        available_modes = {CPUMode::A64};
        reg_map =
        {
            {"x0", X0},
            {"x1", X1},
            {"x2", X2},
            {"x3", X3},
            {"x4", X4},
            {"x5", X5},
            {"x6", X6},
            {"x7", X7},
            {"x8", X8},
            {"x9", X9},
            {"x10", X10},
            {"x11", X11},
            {"x12", X12},
            {"x13", X13},
            {"x14", X14},
            {"x15", X15},
            {"x16", X16},
            {"x17", X17},
            {"x18", X18},
            {"x19", X19},
            {"x20", X20},
            {"x21", X21},
            {"x22", X22},
            {"x23", X23},
            {"x24", X24},
            {"x25", X25},
            {"x26", X26},
            {"x27", X27},
            {"x28", X28},
            {"x29", X29},
            {"x30", X30},
            {"sp", SP},
            {"pc", PC},
            {"zr", ZR},
            {"xzr", ZR}, // Alternative name for zero register
            {"nf", NF},
            {"zf", ZF},
            {"cf", CF},
            {"vf", VF},
            {"cntpct_el0", CNTPCT_EL0},
            {"fp", X29}, // Frame pointer alias for X29
            {"lr", X30}, // Link register alias for X30
            {"nzcv", NZCV}
        };
    }

    size_t ArchARM64::reg_size(reg_t reg_num) const
    {
        switch (reg_num)
        {
            // 64-bit general purpose registers
            case X0:
            case X1:
            case X2:
            case X3:
            case X4:
            case X5:
            case X6:
            case X7:
            case X8:
            case X9:
            case X10:
            case X11:
            case X12:
            case X13:
            case X14:
            case X15:
            case X16:
            case X17:
            case X18:
            case X19:
            case X20:
            case X21:
            case X22:
            case X23:
            case X24:
            case X25:
            case X26:
            case X27:
            case X28:
            case X29:
            case X30:
            case SP:
            case PC:
            case ZR:
                return 64;
            // Status flags (represented as bytes in sleigh/pcode)
            case NF:
            case ZF:
            case CF:
            case VF:
                return 8;
            // System registers
            case CNTPCT_EL0:
                return 64;
            // Composite status register
            case NZCV:
                return 32;
            default:
                throw runtime_exception("ArchARM64::reg_size(): got unsupported reg num");
        }
    }

    reg_t ArchARM64::sp() const
    {
        return ARM64::SP;
    }

    reg_t ArchARM64::pc() const
    {
        return ARM64::PC;
    }

    reg_t ArchARM64::tsc() const
    {
        return ARM64::CNTPCT_EL0;
    }

} // namespace ARM64

} // namespace maat