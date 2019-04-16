#include "chainerx/native/native_device.h"

#include <cstdint>

#include "chainerx/array.h"
#include "chainerx/device.h"
#include "chainerx/dtype.h"
#include "chainerx/native/elementwise.h"
#include "chainerx/native/op_regist.h"
#include "chainerx/routines/creation.h"
#include "chainerx/routines/misc.h"

namespace chainerx {
namespace native {
namespace {

CHAINERX_NATIVE_REGISTER_ELTWISE_UNARY_OP(CopyOp, { out = x; });

class NativeAsTypeOp : public AsTypeOp {
public:
    void Call(const Array& a, const Array& out) override {
        a.device().CheckDevicesCompatible(a, out);
        auto do_astype = [&](auto in_pt, auto out_pt) {
            using InT = typename decltype(in_pt)::type;
            using OutT = typename decltype(out_pt)::type;
            struct Impl {
                void operator()(int64_t /*i*/, InT a, OutT& out) { out = static_cast<OutT>(a); }
            };
            Elementwise<const InT, OutT>(Impl{}, a, out);
        };
        VisitDtype(out.dtype(), [&](auto out_pt) { VisitDtype(a.dtype(), do_astype, out_pt); });
    }
};

CHAINERX_NATIVE_REGISTER_OP(AsTypeOp, NativeAsTypeOp);

}  // namespace
}  // namespace native
}  // namespace chainerx
