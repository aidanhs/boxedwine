// ADD Eb,Gb
BOOL decode000(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = addr8r8;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("ADD", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = adde8r8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("ADD", M8(data, rm, data->op),R8(data->op->r1));
    } else {
        data->op->func = adde8r8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("ADD", M8(data, rm, data->op),R8(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// ADD Ew,Gw
BOOL decode001(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = addr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("ADD", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = adde16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("ADD", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = adde16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("ADD", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// ADD Ed,Gd
BOOL decode201(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = addr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("ADD", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = adde32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("ADD", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = adde32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("ADD", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// ADD Gb,Eb
BOOL decode002(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = addr8r8;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("ADD", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = addr8e8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("ADD", R8(data->op->r1),M8(data, rm, data->op));
    } else {
        data->op->func = addr8e8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("ADD", R8(data->op->r1),M8(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// ADD Gw,Ew
BOOL decode003(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = addr16r16;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("ADD", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = addr16e16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("ADD", R16(data->op->r1),M16(data, rm, data->op));
    } else {
        data->op->func = addr16e16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("ADD", R16(data->op->r1),M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// ADD Gd,Ed
BOOL decode203(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = addr32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("ADD", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = addr32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("ADD", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = addr32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("ADD", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// ADD Al,Ib
BOOL decode004(struct DecodeData* data) {
    data->op->func = add8_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH8(data);
    LOG_OP2("ADD", R8(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// ADD Ax,Iw
BOOL decode005(struct DecodeData* data) {
    data->op->func = add16_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH16(data);
    LOG_OP2("ADD", R16(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// ADD Eax,Id
BOOL decode205(struct DecodeData* data) {
    data->op->func = add32_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH32(data);
    LOG_OP2("ADD", R32(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// PUSH ES
BOOL decode006(struct DecodeData* data) {
    data->op->func = pushSeg16;
    data->op->r1 = ES;
    LOG_OP1("PUSH", EABASE(data->op->r1));
    NEXT_OP(data);
    return TRUE;
}
// PUSH ES
BOOL decode206(struct DecodeData* data) {
    data->op->func = pushSeg32;
    data->op->r1 = ES;
    LOG_OP1("PUSH", EABASE(data->op->r1));
    NEXT_OP(data);
    return TRUE;
}
// POP ES
BOOL decode007(struct DecodeData* data) {
    data->op->func = popSeg16;
    data->op->r1 = ES;
    LOG_OP1("POP", EABASE(data->op->r1));
    NEXT_OP(data);
    return TRUE;
}
// POP ES
BOOL decode207(struct DecodeData* data) {
    data->op->func = popSeg32;
    data->op->r1 = ES;
    LOG_OP1("POP", EABASE(data->op->r1));
    NEXT_OP(data);
    return TRUE;
}
// OR Eb,Gb
BOOL decode008(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = orr8r8;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("OR", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = ore8r8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("OR", M8(data, rm, data->op),R8(data->op->r1));
    } else {
        data->op->func = ore8r8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("OR", M8(data, rm, data->op),R8(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// OR Ew,Gw
BOOL decode009(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = orr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("OR", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = ore16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("OR", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = ore16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("OR", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// OR Ed,Gd
BOOL decode209(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = orr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("OR", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = ore32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("OR", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = ore32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("OR", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// OR Gb,Eb
BOOL decode00a(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = orr8r8;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("OR", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = orr8e8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("OR", R8(data->op->r1),M8(data, rm, data->op));
    } else {
        data->op->func = orr8e8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("OR", R8(data->op->r1),M8(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// OR Gw,Ew
BOOL decode00b(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = orr16r16;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("OR", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = orr16e16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("OR", R16(data->op->r1),M16(data, rm, data->op));
    } else {
        data->op->func = orr16e16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("OR", R16(data->op->r1),M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// OR Gd,Ed
BOOL decode20b(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = orr32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("OR", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = orr32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("OR", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = orr32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("OR", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// OR Al,Ib
BOOL decode00c(struct DecodeData* data) {
    data->op->func = or8_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH8(data);
    LOG_OP2("OR", R8(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// OR Ax,Iw
BOOL decode00d(struct DecodeData* data) {
    data->op->func = or16_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH16(data);
    LOG_OP2("OR", R16(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// OR Eax,Id
BOOL decode20d(struct DecodeData* data) {
    data->op->func = or32_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH32(data);
    LOG_OP2("OR", R32(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// PUSH CS
BOOL decode00e(struct DecodeData* data) {
    data->op->func = pushSeg16;
    data->op->r1 = CS;
    LOG_OP1("PUSH", EABASE(data->op->r1));
    NEXT_OP(data);
    return TRUE;
}
// PUSH CS
BOOL decode20e(struct DecodeData* data) {
    data->op->func = pushSeg32;
    data->op->r1 = CS;
    LOG_OP1("PUSH", EABASE(data->op->r1));
    NEXT_OP(data);
    return TRUE;
}
// ADC Eb,Gb
BOOL decode010(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = adcr8r8;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("ADC", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = adce8r8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("ADC", M8(data, rm, data->op),R8(data->op->r1));
    } else {
        data->op->func = adce8r8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("ADC", M8(data, rm, data->op),R8(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// ADC Ew,Gw
BOOL decode011(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = adcr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("ADC", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = adce16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("ADC", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = adce16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("ADC", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// ADC Ed,Gd
BOOL decode211(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = adcr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("ADC", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = adce32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("ADC", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = adce32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("ADC", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// ADC Gb,Eb
BOOL decode012(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = adcr8r8;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("ADC", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = adcr8e8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("ADC", R8(data->op->r1),M8(data, rm, data->op));
    } else {
        data->op->func = adcr8e8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("ADC", R8(data->op->r1),M8(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// ADC Gw,Ew
BOOL decode013(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = adcr16r16;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("ADC", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = adcr16e16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("ADC", R16(data->op->r1),M16(data, rm, data->op));
    } else {
        data->op->func = adcr16e16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("ADC", R16(data->op->r1),M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// ADC Gd,Ed
BOOL decode213(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = adcr32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("ADC", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = adcr32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("ADC", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = adcr32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("ADC", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// ADC Al,Ib
BOOL decode014(struct DecodeData* data) {
    data->op->func = adc8_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH8(data);
    LOG_OP2("ADC", R8(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// ADC Ax,Iw
BOOL decode015(struct DecodeData* data) {
    data->op->func = adc16_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH16(data);
    LOG_OP2("ADC", R16(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// ADC Eax,Id
BOOL decode215(struct DecodeData* data) {
    data->op->func = adc32_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH32(data);
    LOG_OP2("ADC", R32(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// PUSH SS
BOOL decode016(struct DecodeData* data) {
    data->op->func = pushSeg16;
    data->op->r1 = SS;
    LOG_OP1("PUSH", EABASE(data->op->r1));
    NEXT_OP(data);
    return TRUE;
}
// PUSH SS
BOOL decode216(struct DecodeData* data) {
    data->op->func = pushSeg32;
    data->op->r1 = SS;
    LOG_OP1("PUSH", EABASE(data->op->r1));
    NEXT_OP(data);
    return TRUE;
}
// POP SS
BOOL decode017(struct DecodeData* data) {
    data->op->func = popSeg16;
    data->op->r1 = SS;
    LOG_OP1("POP", EABASE(data->op->r1));
    NEXT_OP(data);
    return TRUE;
}
// POP SS
BOOL decode217(struct DecodeData* data) {
    data->op->func = popSeg32;
    data->op->r1 = SS;
    LOG_OP1("POP", EABASE(data->op->r1));
    NEXT_OP(data);
    return TRUE;
}
// SBB Eb,Gb
BOOL decode018(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = sbbr8r8;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("SBB", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = sbbe8r8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("SBB", M8(data, rm, data->op),R8(data->op->r1));
    } else {
        data->op->func = sbbe8r8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("SBB", M8(data, rm, data->op),R8(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// SBB Ew,Gw
BOOL decode019(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = sbbr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("SBB", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = sbbe16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("SBB", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = sbbe16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("SBB", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// SBB Ed,Gd
BOOL decode219(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = sbbr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("SBB", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = sbbe32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("SBB", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = sbbe32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("SBB", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// SBB Gb,Eb
BOOL decode01a(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = sbbr8r8;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("SBB", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = sbbr8e8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("SBB", R8(data->op->r1),M8(data, rm, data->op));
    } else {
        data->op->func = sbbr8e8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("SBB", R8(data->op->r1),M8(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// SBB Gw,Ew
BOOL decode01b(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = sbbr16r16;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("SBB", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = sbbr16e16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("SBB", R16(data->op->r1),M16(data, rm, data->op));
    } else {
        data->op->func = sbbr16e16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("SBB", R16(data->op->r1),M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// SBB Gd,Ed
BOOL decode21b(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = sbbr32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("SBB", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = sbbr32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("SBB", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = sbbr32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("SBB", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// SBB Al,Ib
BOOL decode01c(struct DecodeData* data) {
    data->op->func = sbb8_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH8(data);
    LOG_OP2("SBB", R8(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// SBB Ax,Iw
BOOL decode01d(struct DecodeData* data) {
    data->op->func = sbb16_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH16(data);
    LOG_OP2("SBB", R16(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// SBB Eax,Id
BOOL decode21d(struct DecodeData* data) {
    data->op->func = sbb32_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH32(data);
    LOG_OP2("SBB", R32(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// PUSH DS
BOOL decode01e(struct DecodeData* data) {
    data->op->func = pushSeg16;
    data->op->r1 = DS;
    LOG_OP1("PUSH", EABASE(data->op->r1));
    NEXT_OP(data);
    return TRUE;
}
// PUSH DS
BOOL decode21e(struct DecodeData* data) {
    data->op->func = pushSeg32;
    data->op->r1 = DS;
    LOG_OP1("PUSH", EABASE(data->op->r1));
    NEXT_OP(data);
    return TRUE;
}
// POP DS
BOOL decode01f(struct DecodeData* data) {
    data->op->func = popSeg16;
    data->op->r1 = DS;
    LOG_OP1("POP", EABASE(data->op->r1));
    NEXT_OP(data);
    return TRUE;
}
// POP DS
BOOL decode21f(struct DecodeData* data) {
    data->op->func = popSeg32;
    data->op->r1 = DS;
    LOG_OP1("POP", EABASE(data->op->r1));
    NEXT_OP(data);
    return TRUE;
}
// AND Eb,Gb
BOOL decode020(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = andr8r8;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("AND", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = ande8r8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("AND", M8(data, rm, data->op),R8(data->op->r1));
    } else {
        data->op->func = ande8r8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("AND", M8(data, rm, data->op),R8(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// AND Ew,Gw
BOOL decode021(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = andr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("AND", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = ande16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("AND", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = ande16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("AND", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// AND Ed,Gd
BOOL decode221(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = andr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("AND", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = ande32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("AND", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = ande32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("AND", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// AND Gb,Eb
BOOL decode022(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = andr8r8;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("AND", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = andr8e8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("AND", R8(data->op->r1),M8(data, rm, data->op));
    } else {
        data->op->func = andr8e8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("AND", R8(data->op->r1),M8(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// AND Gw,Ew
BOOL decode023(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = andr16r16;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("AND", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = andr16e16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("AND", R16(data->op->r1),M16(data, rm, data->op));
    } else {
        data->op->func = andr16e16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("AND", R16(data->op->r1),M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// AND Gd,Ed
BOOL decode223(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = andr32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("AND", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = andr32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("AND", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = andr32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("AND", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// AND Al,Ib
BOOL decode024(struct DecodeData* data) {
    data->op->func = and8_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH8(data);
    LOG_OP2("AND", R8(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// AND Ax,Iw
BOOL decode025(struct DecodeData* data) {
    data->op->func = and16_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH16(data);
    LOG_OP2("AND", R16(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// AND Eax,Id
BOOL decode225(struct DecodeData* data) {
    data->op->func = and32_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH32(data);
    LOG_OP2("AND", R32(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// SEG ES
BOOL decode026(struct DecodeData* data) {
    data->ds = ES;
    data->ss = ES;
    return TRUE;
}
// DAA
BOOL decode027(struct DecodeData* data) {
    data->op->func = daa;
    LOG_OP("DAA");
    NEXT_OP(data);
    return TRUE;
}
// SUB Eb,Gb
BOOL decode028(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = subr8r8;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("SUB", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = sube8r8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("SUB", M8(data, rm, data->op),R8(data->op->r1));
    } else {
        data->op->func = sube8r8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("SUB", M8(data, rm, data->op),R8(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// SUB Ew,Gw
BOOL decode029(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = subr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("SUB", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = sube16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("SUB", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = sube16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("SUB", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// SUB Ed,Gd
BOOL decode229(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = subr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("SUB", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = sube32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("SUB", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = sube32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("SUB", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// SUB Gb,Eb
BOOL decode02a(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = subr8r8;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("SUB", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = subr8e8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("SUB", R8(data->op->r1),M8(data, rm, data->op));
    } else {
        data->op->func = subr8e8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("SUB", R8(data->op->r1),M8(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// SUB Gw,Ew
BOOL decode02b(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = subr16r16;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("SUB", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = subr16e16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("SUB", R16(data->op->r1),M16(data, rm, data->op));
    } else {
        data->op->func = subr16e16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("SUB", R16(data->op->r1),M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// SUB Gd,Ed
BOOL decode22b(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = subr32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("SUB", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = subr32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("SUB", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = subr32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("SUB", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// SUB Al,Ib
BOOL decode02c(struct DecodeData* data) {
    data->op->func = sub8_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH8(data);
    LOG_OP2("SUB", R8(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// SUB Ax,Iw
BOOL decode02d(struct DecodeData* data) {
    data->op->func = sub16_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH16(data);
    LOG_OP2("SUB", R16(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// SUB Eax,Id
BOOL decode22d(struct DecodeData* data) {
    data->op->func = sub32_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH32(data);
    LOG_OP2("SUB", R32(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// SEG CS
BOOL decode02e(struct DecodeData* data) {
    data->ds = CS;
    data->ss = CS;
    return TRUE;
}
// DAS
BOOL decode02f(struct DecodeData* data) {
    data->op->func = das;
    LOG_OP("DAS");
    NEXT_OP(data);
    return TRUE;
}
// XOR Eb,Gb
BOOL decode030(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = xorr8r8;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("XOR", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = xore8r8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("XOR", M8(data, rm, data->op),R8(data->op->r1));
    } else {
        data->op->func = xore8r8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("XOR", M8(data, rm, data->op),R8(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// XOR Ew,Gw
BOOL decode031(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = xorr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("XOR", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = xore16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("XOR", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = xore16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("XOR", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// XOR Ed,Gd
BOOL decode231(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = xorr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("XOR", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = xore32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("XOR", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = xore32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("XOR", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// XOR Gb,Eb
BOOL decode032(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = xorr8r8;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("XOR", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = xorr8e8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("XOR", R8(data->op->r1),M8(data, rm, data->op));
    } else {
        data->op->func = xorr8e8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("XOR", R8(data->op->r1),M8(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// XOR Gw,Ew
BOOL decode033(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = xorr16r16;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("XOR", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = xorr16e16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("XOR", R16(data->op->r1),M16(data, rm, data->op));
    } else {
        data->op->func = xorr16e16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("XOR", R16(data->op->r1),M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// XOR Gd,Ed
BOOL decode233(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = xorr32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("XOR", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = xorr32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("XOR", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = xorr32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("XOR", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// XOR Al,Ib
BOOL decode034(struct DecodeData* data) {
    data->op->func = xor8_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH8(data);
    LOG_OP2("XOR", R8(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// XOR Ax,Iw
BOOL decode035(struct DecodeData* data) {
    data->op->func = xor16_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH16(data);
    LOG_OP2("XOR", R16(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// XOR Eax,Id
BOOL decode235(struct DecodeData* data) {
    data->op->func = xor32_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH32(data);
    LOG_OP2("XOR", R32(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// SEG SS
BOOL decode036(struct DecodeData* data) {
    data->ds = SS;
    data->ss = SS;
    return TRUE;
}
// AAA
BOOL decode037(struct DecodeData* data) {
    data->op->func = aaa;
    LOG_OP("AAA");
    NEXT_OP(data);
    return TRUE;
}
// CMP Eb,Gb
BOOL decode038(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmpr8r8;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("CMP", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmpe8r8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMP", M8(data, rm, data->op),R8(data->op->r1));
    } else {
        data->op->func = cmpe8r8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMP", M8(data, rm, data->op),R8(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMP Ew,Gw
BOOL decode039(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmpr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("CMP", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmpe16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMP", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = cmpe16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMP", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMP Ed,Gd
BOOL decode239(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmpr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("CMP", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmpe32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMP", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = cmpe32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMP", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMP Gb,Eb
BOOL decode03a(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmpr8r8;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMP", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmpr8e8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMP", R8(data->op->r1),M8(data, rm, data->op));
    } else {
        data->op->func = cmpr8e8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMP", R8(data->op->r1),M8(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMP Gw,Ew
BOOL decode03b(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmpr16r16;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMP", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmpr16e16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMP", R16(data->op->r1),M16(data, rm, data->op));
    } else {
        data->op->func = cmpr16e16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMP", R16(data->op->r1),M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMP Gd,Ed
BOOL decode23b(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmpr32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMP", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmpr32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMP", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = cmpr32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMP", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMP Al,Ib
BOOL decode03c(struct DecodeData* data) {
    data->op->func = cmp8_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH8(data);
    LOG_OP2("CMP", R8(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// CMP Ax,Iw
BOOL decode03d(struct DecodeData* data) {
    data->op->func = cmp16_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH16(data);
    LOG_OP2("CMP", R16(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// CMP Eax,Id
BOOL decode23d(struct DecodeData* data) {
    data->op->func = cmp32_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH32(data);
    LOG_OP2("CMP", R32(0), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// SEG DS
BOOL decode03e(struct DecodeData* data) {
    data->ds = DS;
    data->ss = DS;
    return TRUE;
}
// AAS
BOOL decode03f(struct DecodeData* data) {
    data->op->func = aas;
    LOG_OP("AAS");
    NEXT_OP(data);
    return TRUE;
}
// INC AX
BOOL decode040(struct DecodeData* data) {
    data->op->func = inc16_reg;
    data->op->r1 = 0;
    LOG_OP("INC AX");
    NEXT_OP(data);
    return TRUE;
}
// INC EAX
BOOL decode240(struct DecodeData* data) {
    data->op->func = inc32_reg;
    data->op->r1 = 0;
    LOG_OP("INC EAX");
    NEXT_OP(data);
    return TRUE;
}
// INC CX
BOOL decode041(struct DecodeData* data) {
    data->op->func = inc16_reg;
    data->op->r1 = 1;
    LOG_OP("INC CX");
    NEXT_OP(data);
    return TRUE;
}
// INC ECX
BOOL decode241(struct DecodeData* data) {
    data->op->func = inc32_reg;
    data->op->r1 = 1;
    LOG_OP("INC ECX");
    NEXT_OP(data);
    return TRUE;
}
// INC DX
BOOL decode042(struct DecodeData* data) {
    data->op->func = inc16_reg;
    data->op->r1 = 2;
    LOG_OP("INC DX");
    NEXT_OP(data);
    return TRUE;
}
// INC EDX
BOOL decode242(struct DecodeData* data) {
    data->op->func = inc32_reg;
    data->op->r1 = 2;
    LOG_OP("INC EDX");
    NEXT_OP(data);
    return TRUE;
}
// INC BX
BOOL decode043(struct DecodeData* data) {
    data->op->func = inc16_reg;
    data->op->r1 = 3;
    LOG_OP("INC BX");
    NEXT_OP(data);
    return TRUE;
}
// INC EBX
BOOL decode243(struct DecodeData* data) {
    data->op->func = inc32_reg;
    data->op->r1 = 3;
    LOG_OP("INC EBX");
    NEXT_OP(data);
    return TRUE;
}
// INC SP
BOOL decode044(struct DecodeData* data) {
    data->op->func = inc16_reg;
    data->op->r1 = 4;
    LOG_OP("INC SP");
    NEXT_OP(data);
    return TRUE;
}
// INC ESP
BOOL decode244(struct DecodeData* data) {
    data->op->func = inc32_reg;
    data->op->r1 = 4;
    LOG_OP("INC ESP");
    NEXT_OP(data);
    return TRUE;
}
// INC BP
BOOL decode045(struct DecodeData* data) {
    data->op->func = inc16_reg;
    data->op->r1 = 5;
    LOG_OP("INC BP");
    NEXT_OP(data);
    return TRUE;
}
// INC EBP
BOOL decode245(struct DecodeData* data) {
    data->op->func = inc32_reg;
    data->op->r1 = 5;
    LOG_OP("INC EBP");
    NEXT_OP(data);
    return TRUE;
}
// INC SI
BOOL decode046(struct DecodeData* data) {
    data->op->func = inc16_reg;
    data->op->r1 = 6;
    LOG_OP("INC SI");
    NEXT_OP(data);
    return TRUE;
}
// INC ESI
BOOL decode246(struct DecodeData* data) {
    data->op->func = inc32_reg;
    data->op->r1 = 6;
    LOG_OP("INC ESI");
    NEXT_OP(data);
    return TRUE;
}
// INC DI
BOOL decode047(struct DecodeData* data) {
    data->op->func = inc16_reg;
    data->op->r1 = 7;
    LOG_OP("INC DI");
    NEXT_OP(data);
    return TRUE;
}
// INC EDI
BOOL decode247(struct DecodeData* data) {
    data->op->func = inc32_reg;
    data->op->r1 = 7;
    LOG_OP("INC EDI");
    NEXT_OP(data);
    return TRUE;
}
// DEC AX
BOOL decode048(struct DecodeData* data) {
    data->op->func = dec16_reg;
    data->op->r1 = 0;
    LOG_OP("DEC AX");
    NEXT_OP(data);
    return TRUE;
}
// DEC EAX
BOOL decode248(struct DecodeData* data) {
    data->op->func = dec32_reg;
    data->op->r1 = 0;
    LOG_OP("DEC EAX");
    NEXT_OP(data);
    return TRUE;
}
// DEC CX
BOOL decode049(struct DecodeData* data) {
    data->op->func = dec16_reg;
    data->op->r1 = 1;
    LOG_OP("DEC CX");
    NEXT_OP(data);
    return TRUE;
}
// DEC ECX
BOOL decode249(struct DecodeData* data) {
    data->op->func = dec32_reg;
    data->op->r1 = 1;
    LOG_OP("DEC ECX");
    NEXT_OP(data);
    return TRUE;
}
// DEC DX
BOOL decode04a(struct DecodeData* data) {
    data->op->func = dec16_reg;
    data->op->r1 = 2;
    LOG_OP("DEC DX");
    NEXT_OP(data);
    return TRUE;
}
// DEC EDX
BOOL decode24a(struct DecodeData* data) {
    data->op->func = dec32_reg;
    data->op->r1 = 2;
    LOG_OP("DEC EDX");
    NEXT_OP(data);
    return TRUE;
}
// DEC BX
BOOL decode04b(struct DecodeData* data) {
    data->op->func = dec16_reg;
    data->op->r1 = 3;
    LOG_OP("DEC BX");
    NEXT_OP(data);
    return TRUE;
}
// DEC EBX
BOOL decode24b(struct DecodeData* data) {
    data->op->func = dec32_reg;
    data->op->r1 = 3;
    LOG_OP("DEC EBX");
    NEXT_OP(data);
    return TRUE;
}
// DEC SP
BOOL decode04c(struct DecodeData* data) {
    data->op->func = dec16_reg;
    data->op->r1 = 4;
    LOG_OP("DEC SP");
    NEXT_OP(data);
    return TRUE;
}
// DEC ESP
BOOL decode24c(struct DecodeData* data) {
    data->op->func = dec32_reg;
    data->op->r1 = 4;
    LOG_OP("DEC ESP");
    NEXT_OP(data);
    return TRUE;
}
// DEC BP
BOOL decode04d(struct DecodeData* data) {
    data->op->func = dec16_reg;
    data->op->r1 = 5;
    LOG_OP("DEC BP");
    NEXT_OP(data);
    return TRUE;
}
// DEC EBP
BOOL decode24d(struct DecodeData* data) {
    data->op->func = dec32_reg;
    data->op->r1 = 5;
    LOG_OP("DEC EBP");
    NEXT_OP(data);
    return TRUE;
}
// DEC SI
BOOL decode04e(struct DecodeData* data) {
    data->op->func = dec16_reg;
    data->op->r1 = 6;
    LOG_OP("DEC SI");
    NEXT_OP(data);
    return TRUE;
}
// DEC ESI
BOOL decode24e(struct DecodeData* data) {
    data->op->func = dec32_reg;
    data->op->r1 = 6;
    LOG_OP("DEC ESI");
    NEXT_OP(data);
    return TRUE;
}
// DEC DI
BOOL decode04f(struct DecodeData* data) {
    data->op->func = dec16_reg;
    data->op->r1 = 7;
    LOG_OP("DEC DI");
    NEXT_OP(data);
    return TRUE;
}
// DEC EDI
BOOL decode24f(struct DecodeData* data) {
    data->op->func = dec32_reg;
    data->op->r1 = 7;
    LOG_OP("DEC EDI");
    NEXT_OP(data);
    return TRUE;
}
// PUSH AX
BOOL decode050(struct DecodeData* data) {
    data->op->func = pushAx;
    data->op->r1 = 0;
    LOG_OP("PUSH AX");
    NEXT_OP(data);
    return TRUE;
}
// PUSH EAX
BOOL decode250(struct DecodeData* data) {
    data->op->func = pushEax;
    data->op->r1 = 0;
    LOG_OP("PUSH EAX");
    NEXT_OP(data);
    return TRUE;
}
// PUSH CX
BOOL decode051(struct DecodeData* data) {
    data->op->func = pushCx;
    data->op->r1 = 1;
    LOG_OP("PUSH CX");
    NEXT_OP(data);
    return TRUE;
}
// PUSH ECX
BOOL decode251(struct DecodeData* data) {
    data->op->func = pushEcx;
    data->op->r1 = 1;
    LOG_OP("PUSH ECX");
    NEXT_OP(data);
    return TRUE;
}
// PUSH DX
BOOL decode052(struct DecodeData* data) {
    data->op->func = pushDx;
    data->op->r1 = 2;
    LOG_OP("PUSH DX");
    NEXT_OP(data);
    return TRUE;
}
// PUSH EDX
BOOL decode252(struct DecodeData* data) {
    data->op->func = pushEdx;
    data->op->r1 = 2;
    LOG_OP("PUSH EDX");
    NEXT_OP(data);
    return TRUE;
}
// PUSH BX
BOOL decode053(struct DecodeData* data) {
    data->op->func = pushBx;
    data->op->r1 = 3;
    LOG_OP("PUSH BX");
    NEXT_OP(data);
    return TRUE;
}
// PUSH EBX
BOOL decode253(struct DecodeData* data) {
    data->op->func = pushEbx;
    data->op->r1 = 3;
    LOG_OP("PUSH EBX");
    NEXT_OP(data);
    return TRUE;
}
// PUSH SP
BOOL decode054(struct DecodeData* data) {
    data->op->func = pushSp;
    data->op->r1 = 4;
    LOG_OP("PUSH SP");
    NEXT_OP(data);
    return TRUE;
}
// PUSH ESP
BOOL decode254(struct DecodeData* data) {
    data->op->func = pushEsp;
    data->op->r1 = 4;
    LOG_OP("PUSH ESP");
    NEXT_OP(data);
    return TRUE;
}
// PUSH BP
BOOL decode055(struct DecodeData* data) {
    data->op->func = pushBp;
    data->op->r1 = 5;
    LOG_OP("PUSH BP");
    NEXT_OP(data);
    return TRUE;
}
// PUSH EBP
BOOL decode255(struct DecodeData* data) {
    data->op->func = pushEbp;
    data->op->r1 = 5;
    LOG_OP("PUSH EBP");
    NEXT_OP(data);
    return TRUE;
}
// PUSH SI
BOOL decode056(struct DecodeData* data) {
    data->op->func = pushSi;
    data->op->r1 = 6;
    LOG_OP("PUSH SI");
    NEXT_OP(data);
    return TRUE;
}
// PUSH ESI
BOOL decode256(struct DecodeData* data) {
    data->op->func = pushEsi;
    data->op->r1 = 6;
    LOG_OP("PUSH ESI");
    NEXT_OP(data);
    return TRUE;
}
// PUSH DI
BOOL decode057(struct DecodeData* data) {
    data->op->func = pushDi;
    data->op->r1 = 7;
    LOG_OP("PUSH DI");
    NEXT_OP(data);
    return TRUE;
}
// PUSH EDI
BOOL decode257(struct DecodeData* data) {
    data->op->func = pushEdi;
    data->op->r1 = 7;
    LOG_OP("PUSH EDI");
    NEXT_OP(data);
    return TRUE;
}
// PUSH AX
BOOL decode058(struct DecodeData* data) {
    data->op->func = popAx;
    data->op->r1 = 0;
    LOG_OP("PUSH AX");
    NEXT_OP(data);
    return TRUE;
}
// POP EAX
BOOL decode258(struct DecodeData* data) {
    data->op->func = popEax;
    data->op->r1 = 0;
    LOG_OP("POP EAX");
    NEXT_OP(data);
    return TRUE;
}
// POP CX
BOOL decode059(struct DecodeData* data) {
    data->op->func = popCx;
    data->op->r1 = 1;
    LOG_OP("POP CX");
    NEXT_OP(data);
    return TRUE;
}
// POP ECX
BOOL decode259(struct DecodeData* data) {
    data->op->func = popEcx;
    data->op->r1 = 1;
    LOG_OP("POP ECX");
    NEXT_OP(data);
    return TRUE;
}
// POP DX
BOOL decode05a(struct DecodeData* data) {
    data->op->func = popDx;
    data->op->r1 = 2;
    LOG_OP("POP DX");
    NEXT_OP(data);
    return TRUE;
}
// POP EDX
BOOL decode25a(struct DecodeData* data) {
    data->op->func = popEdx;
    data->op->r1 = 2;
    LOG_OP("POP EDX");
    NEXT_OP(data);
    return TRUE;
}
// POP BX
BOOL decode05b(struct DecodeData* data) {
    data->op->func = popBx;
    data->op->r1 = 3;
    LOG_OP("POP BX");
    NEXT_OP(data);
    return TRUE;
}
// POP EBX
BOOL decode25b(struct DecodeData* data) {
    data->op->func = popEbx;
    data->op->r1 = 3;
    LOG_OP("POP EBX");
    NEXT_OP(data);
    return TRUE;
}
// POP SP
BOOL decode05c(struct DecodeData* data) {
    data->op->func = popSp;
    data->op->r1 = 4;
    LOG_OP("POP SP");
    NEXT_OP(data);
    return TRUE;
}
// POP ESP
BOOL decode25c(struct DecodeData* data) {
    data->op->func = popEsp;
    data->op->r1 = 4;
    LOG_OP("POP ESP");
    NEXT_OP(data);
    return TRUE;
}
// POP BP
BOOL decode05d(struct DecodeData* data) {
    data->op->func = popBp;
    data->op->r1 = 5;
    LOG_OP("POP BP");
    NEXT_OP(data);
    return TRUE;
}
// POP EBP
BOOL decode25d(struct DecodeData* data) {
    data->op->func = popEbp;
    data->op->r1 = 5;
    LOG_OP("POP EBP");
    NEXT_OP(data);
    return TRUE;
}
// POP SI
BOOL decode05e(struct DecodeData* data) {
    data->op->func = popSi;
    data->op->r1 = 6;
    LOG_OP("POP SI");
    NEXT_OP(data);
    return TRUE;
}
// POP ESI
BOOL decode25e(struct DecodeData* data) {
    data->op->func = popEsi;
    data->op->r1 = 6;
    LOG_OP("POP ESI");
    NEXT_OP(data);
    return TRUE;
}
// POP DI
BOOL decode05f(struct DecodeData* data) {
    data->op->func = popDi;
    data->op->r1 = 7;
    LOG_OP("POP DI");
    NEXT_OP(data);
    return TRUE;
}
// POP EDI
BOOL decode25f(struct DecodeData* data) {
    data->op->func = popEdi;
    data->op->r1 = 7;
    LOG_OP("POP EDI");
    NEXT_OP(data);
    return TRUE;
}
// PUSHA
BOOL decode060(struct DecodeData* data) {
    data->op->func = pusha;
    LOG_OP("PUSHA");
    NEXT_OP(data);
    return TRUE;
}
// PUSHAD
BOOL decode260(struct DecodeData* data) {
    data->op->func = pushad;
    LOG_OP("PUSHA");
    NEXT_OP(data);
    return TRUE;
}
// POPA
BOOL decode061(struct DecodeData* data) {
    data->op->func = popa;
    LOG_OP("POPA");
    NEXT_OP(data);
    return TRUE;
}
// POPAD
BOOL decode261(struct DecodeData* data) {
    data->op->func = popad;
    LOG_OP("POPA");
    NEXT_OP(data);
    return TRUE;
}
// SEG FS
BOOL decode064(struct DecodeData* data) {
    data->ds = FS;
    data->ss = FS;
    return TRUE;
}
// SEG GS
BOOL decode065(struct DecodeData* data) {
    data->ds = GS;
    data->ss = GS;
    return TRUE;
}
// PUSH Iw
BOOL decode068(struct DecodeData* data) {
    data->op->func = push16data;
    data->op->data1 = FETCH16(data);
    LOG_OP1("PUSH", itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// PUSH Id
BOOL decode268(struct DecodeData* data) {
    data->op->func = push32data;
    data->op->data1 = FETCH32(data);
    LOG_OP1("PUSH", itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// IMUL Gw,Ew,Iw
BOOL decode069(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = dimulcr16r16;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("DIMULC", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = dimulcr16e16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("DIMULC", R16(data->op->r1),M16(data, rm, data->op));
    } else {
        data->op->func = dimulcr16e16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("DIMULC", R16(data->op->r1),M16(data, rm, data->op));
    }
    data->op->data1 = FETCH_S16(data);
    NEXT_OP(data);
    return TRUE;
}
// IMUL Gd,Ed,Id
BOOL decode269(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = dimulcr32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("DIMULC", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = dimulcr32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("DIMULC", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = dimulcr32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("DIMULC", R32(data->op->r1),M32(data, rm, data->op));
    }
    data->op->data1 = FETCH32(data);
    NEXT_OP(data);
    return TRUE;
}
// PUSH Ib
BOOL decode06a(struct DecodeData* data) {
    data->op->func = push16data;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("PUSH", itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// PUSH Ib
BOOL decode26a(struct DecodeData* data) {
    data->op->func = push32data;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("PUSH", itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// IMUL Gw,Ew,Ib
BOOL decode06b(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = dimulcr16r16;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("DIMULC", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = dimulcr16e16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("DIMULC", R16(data->op->r1),M16(data, rm, data->op));
    } else {
        data->op->func = dimulcr16e16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("DIMULC", R16(data->op->r1),M16(data, rm, data->op));
    }
    data->op->data1 = FETCH_S8(data);
    NEXT_OP(data);
    return TRUE;
}
// IMUL Gd,Ed,Ib
BOOL decode26b(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = dimulcr32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("DIMULC", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = dimulcr32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("DIMULC", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = dimulcr32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("DIMULC", R32(data->op->r1),M32(data, rm, data->op));
    }
    data->op->data1 = FETCH_S8(data);
    NEXT_OP(data);
    return TRUE;
}
// JO
BOOL decode070(struct DecodeData* data) {
    data->op->func = jumpO;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JO", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNO
BOOL decode071(struct DecodeData* data) {
    data->op->func = jumpNO;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JNO", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JB
BOOL decode072(struct DecodeData* data) {
    data->op->func = jumpB;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JB", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNB
BOOL decode073(struct DecodeData* data) {
    data->op->func = jumpNB;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JNB", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JZ
BOOL decode074(struct DecodeData* data) {
    data->op->func = jumpZ;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JZ", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNZ
BOOL decode075(struct DecodeData* data) {
    data->op->func = jumpNZ;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JNZ", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JBE
BOOL decode076(struct DecodeData* data) {
    data->op->func = jumpBE;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JBE", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNBE
BOOL decode077(struct DecodeData* data) {
    data->op->func = jumpNBE;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JNBE", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JS
BOOL decode078(struct DecodeData* data) {
    data->op->func = jumpS;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JS", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNS
BOOL decode079(struct DecodeData* data) {
    data->op->func = jumpNS;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JNS", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JP
BOOL decode07a(struct DecodeData* data) {
    data->op->func = jumpP;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JP", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNP
BOOL decode07b(struct DecodeData* data) {
    data->op->func = jumpNP;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JNP", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JL
BOOL decode07c(struct DecodeData* data) {
    data->op->func = jumpL;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JL", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNL
BOOL decode07d(struct DecodeData* data) {
    data->op->func = jumpNL;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JNL", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JLE
BOOL decode07e(struct DecodeData* data) {
    data->op->func = jumpLE;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JLE", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNLE
BOOL decode07f(struct DecodeData* data) {
    data->op->func = jumpNLE;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JNLE", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// TEST Eb,Gb
BOOL decode084(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = testr8r8;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("TEST", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = teste8r8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("TEST", M8(data, rm, data->op),R8(data->op->r1));
    } else {
        data->op->func = teste8r8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("TEST", M8(data, rm, data->op),R8(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// TEST Ew,Gw
BOOL decode085(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = testr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("TEST", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = teste16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("TEST", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = teste16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("TEST", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// TEST Ed,Gd
BOOL decode285(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = testr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("TEST", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = teste32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("TEST", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = teste32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("TEST", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// XCHG Eb,Gb
BOOL decode086(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = xchgr8r8;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("XCHG", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = xchge8r8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("XCHG", M8(data, rm, data->op),R8(data->op->r1));
    } else {
        data->op->func = xchge8r8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("XCHG", M8(data, rm, data->op),R8(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// XCHG Ew,Gw
BOOL decode087(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = xchgr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("XCHG", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = xchge16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("XCHG", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = xchge16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("XCHG", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// XCHG Ed,Gd
BOOL decode287(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = xchgr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("XCHG", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = xchge32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("XCHG", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = xchge32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("XCHG", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// MOV Eb,Gb
BOOL decode088(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = movr8r8;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("MOV", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = move8r8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("MOV", M8(data, rm, data->op),R8(data->op->r1));
    } else {
        data->op->func = move8r8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("MOV", M8(data, rm, data->op),R8(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// MOV Ew,Gw
BOOL decode089(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = movr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("MOV", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = move16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("MOV", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = move16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("MOV", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// MOV Ed,Gd
BOOL decode289(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = movr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("MOV", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = move32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("MOV", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = move32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("MOV", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// MOV Gb,Eb
BOOL decode08a(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = movr8r8;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("MOV", R8(data->op->r1),R8(data->op->r2));
    } else if (data->ea16) {
        data->op->func = movr8e8_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("MOV", R8(data->op->r1),M8(data, rm, data->op));
    } else {
        data->op->func = movr8e8_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("MOV", R8(data->op->r1),M8(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// MOV Gw,Ew
BOOL decode08b(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = movr16r16;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("MOV", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = movr16e16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("MOV", R16(data->op->r1),M16(data, rm, data->op));
    } else {
        data->op->func = movr16e16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("MOV", R16(data->op->r1),M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// MOV Gd,Ed
BOOL decode28b(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = movr32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("MOV", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = movr32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("MOV", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = movr32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("MOV", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// XCHG CX,AX
BOOL decode091(struct DecodeData* data) {
    data->op->func = xchgr16r16;
    data->op->r1 = 0;
    data->op->r2 = 1;
    NEXT_OP(data);
    return TRUE;
}
// XCHG ECX,EAX
BOOL decode291(struct DecodeData* data) {
    data->op->func = xchgr32r32;
    data->op->r1 = 0;
    data->op->r2 = 1;
    NEXT_OP(data);
    return TRUE;
}
// XCHG DX,AX
BOOL decode092(struct DecodeData* data) {
    data->op->func = xchgr16r16;
    data->op->r1 = 0;
    data->op->r2 = 2;
    NEXT_OP(data);
    return TRUE;
}
// XCHG EDX,EAX
BOOL decode292(struct DecodeData* data) {
    data->op->func = xchgr32r32;
    data->op->r1 = 0;
    data->op->r2 = 2;
    NEXT_OP(data);
    return TRUE;
}
// XCHG BX,AX
BOOL decode093(struct DecodeData* data) {
    data->op->func = xchgr16r16;
    data->op->r1 = 0;
    data->op->r2 = 3;
    NEXT_OP(data);
    return TRUE;
}
// XCHG EBX,EAX
BOOL decode293(struct DecodeData* data) {
    data->op->func = xchgr32r32;
    data->op->r1 = 0;
    data->op->r2 = 3;
    NEXT_OP(data);
    return TRUE;
}
// XCHG SP,AX
BOOL decode094(struct DecodeData* data) {
    data->op->func = xchgr16r16;
    data->op->r1 = 0;
    data->op->r2 = 4;
    NEXT_OP(data);
    return TRUE;
}
// XCHG ESP,EAX
BOOL decode294(struct DecodeData* data) {
    data->op->func = xchgr32r32;
    data->op->r1 = 0;
    data->op->r2 = 4;
    NEXT_OP(data);
    return TRUE;
}
// XCHG BP,AX
BOOL decode095(struct DecodeData* data) {
    data->op->func = xchgr16r16;
    data->op->r1 = 0;
    data->op->r2 = 5;
    NEXT_OP(data);
    return TRUE;
}
// XCHG EBP,EAX
BOOL decode295(struct DecodeData* data) {
    data->op->func = xchgr32r32;
    data->op->r1 = 0;
    data->op->r2 = 5;
    NEXT_OP(data);
    return TRUE;
}
// XCHG SI,AX
BOOL decode096(struct DecodeData* data) {
    data->op->func = xchgr16r16;
    data->op->r1 = 0;
    data->op->r2 = 6;
    NEXT_OP(data);
    return TRUE;
}
// XCHG ESI,EAX
BOOL decode296(struct DecodeData* data) {
    data->op->func = xchgr32r32;
    data->op->r1 = 0;
    data->op->r2 = 6;
    NEXT_OP(data);
    return TRUE;
}
// XCHG DI,AX
BOOL decode097(struct DecodeData* data) {
    data->op->func = xchgr16r16;
    data->op->r1 = 0;
    data->op->r2 = 7;
    NEXT_OP(data);
    return TRUE;
}
// XCHG EDI,EAX
BOOL decode297(struct DecodeData* data) {
    data->op->func = xchgr32r32;
    data->op->r1 = 0;
    data->op->r2 = 7;
    NEXT_OP(data);
    return TRUE;
}
// CBW
BOOL decode098(struct DecodeData* data) {
    data->op->func = cbw;
    LOG_OP("CBW");
    NEXT_OP(data);
    return TRUE;
}
// CBWE
BOOL decode298(struct DecodeData* data) {
    data->op->func = cbwe;
    LOG_OP("CBWE");
    NEXT_OP(data);
    return TRUE;
}
// CWD
BOOL decode099(struct DecodeData* data) {
    data->op->func = cwd;
    LOG_OP("CWD");
    NEXT_OP(data);
    return TRUE;
}
// CWQ
BOOL decode299(struct DecodeData* data) {
    data->op->func = cwq;
    LOG_OP("CWQ");
    NEXT_OP(data);
    return TRUE;
}
// PUSHF16
BOOL decode09c(struct DecodeData* data) {
    data->op->func = pushf16;
    LOG_OP("PUSHF");
    NEXT_OP(data);
    return TRUE;
}
// PUSHF32
BOOL decode29c(struct DecodeData* data) {
    data->op->func = pushf32;
    LOG_OP("PUSHF");
    NEXT_OP(data);
    return TRUE;
}
// POPF16
BOOL decode09d(struct DecodeData* data) {
    data->op->func = popf16;
    LOG_OP("POPF");
    NEXT_OP(data);
    return TRUE;
}
// POPF32
BOOL decode29d(struct DecodeData* data) {
    data->op->func = popf32;
    LOG_OP("POPF");
    NEXT_OP(data);
    return TRUE;
}
// SAHF
BOOL decode09e(struct DecodeData* data) {
    data->op->func = sahf;
    LOG_OP("SAHF");
    NEXT_OP(data);
    return TRUE;
}
// LAHF
BOOL decode09f(struct DecodeData* data) {
    data->op->func = lahf;
    LOG_OP("LAHF");
    NEXT_OP(data);
    return TRUE;
}
// MOV AL,Ob
BOOL decode0a0(struct DecodeData* data) {
    data->op->func = movAl;
    if (data->ea16) {
        data->op->data1 = FETCH16(data);
    } else {
        data->op->data1 = FETCH32(data);
    }
    data->op->base = data->ds;
    LOG_OP2("MOV", "AL", O8(data->op));
    NEXT_OP(data);
    return TRUE;
}
// MOV AX,Ow
BOOL decode0a1(struct DecodeData* data) {
    data->op->func = movAx;
    if (data->ea16) {
        data->op->data1 = FETCH16(data);
    } else {
        data->op->data1 = FETCH32(data);
    }
    data->op->base = data->ds;
    LOG_OP2("MOV", "AX", O16(data->op));
    NEXT_OP(data);
    return TRUE;
}
// MOV EAX,Od
BOOL decode2a1(struct DecodeData* data) {
    data->op->func = movEax;
    if (data->ea16) {
        data->op->data1 = FETCH16(data);
    } else {
        data->op->data1 = FETCH32(data);
    }
    data->op->base = data->ds;
    LOG_OP2("MOV", "EAX", O32(data->op));
    NEXT_OP(data);
    return TRUE;
}
// MOV Ob,Al
BOOL decode0a2(struct DecodeData* data) {
    data->op->func = movDirectAl;
    if (data->ea16) {
        data->op->data1 = FETCH16(data);
    } else {
        data->op->data1 = FETCH32(data);
    }
    data->op->base = data->ds;
    LOG_OP2("MOV", O8(data->op), "AL");
    NEXT_OP(data);
    return TRUE;
}
// MOV Ow,Ax
BOOL decode0a3(struct DecodeData* data) {
    data->op->func = movDirectAx;
    if (data->ea16) {
        data->op->data1 = FETCH16(data);
    } else {
        data->op->data1 = FETCH32(data);
    }
    data->op->base = data->ds;
    LOG_OP2("MOV", O16(data->op), "AX");
    NEXT_OP(data);
    return TRUE;
}
// MOV Od,Eax
BOOL decode2a3(struct DecodeData* data) {
    data->op->func = movDirectEax;
    if (data->ea16) {
        data->op->data1 = FETCH16(data);
    } else {
        data->op->data1 = FETCH32(data);
    }
    data->op->base = data->ds;
    LOG_OP2("MOV", O32(data->op), "EAX");
    NEXT_OP(data);
    return TRUE;
}
// MOVSB
BOOL decode0a4(struct DecodeData* data) {
    if (data->ea16) {
        if (data->rep) {
            data->op->func = movsb16_r;
        } else {
            data->op->func = movsb16;
        }
    } else {
        if (data->rep) {
            data->op->func = movsb32_r;
        } else {
            data->op->func = movsb32;
        }
    }
    data->op->data1 = data->rep_zero;
    NEXT_OP(data);
    return TRUE;
}
// MOVSW
BOOL decode0a5(struct DecodeData* data) {
    if (data->ea16) {
        if (data->rep) {
            data->op->func = movsw16_r;
        } else {
            data->op->func = movsw16;
        }
    } else {
        if (data->rep) {
            data->op->func = movsw32_r;
        } else {
            data->op->func = movsw32;
        }
    }
    data->op->data1 = data->rep_zero;
    NEXT_OP(data);
    return TRUE;
}
// MOVSD
BOOL decode2a5(struct DecodeData* data) {
    if (data->ea16) {
        if (data->rep) {
            data->op->func = movsd16_r;
        } else {
            data->op->func = movsd16;
        }
    } else {
        if (data->rep) {
            data->op->func = movsd32_r;
        } else {
            data->op->func = movsd32;
        }
    }
    data->op->data1 = data->rep_zero;
    NEXT_OP(data);
    return TRUE;
}
// CMPSB
BOOL decode0a6(struct DecodeData* data) {
    if (data->ea16) {
        if (data->rep) {
            data->op->func = cmpsb16_r;
        } else {
            data->op->func = cmpsb16;
        }
    } else {
        if (data->rep) {
            data->op->func = cmpsb32_r;
        } else {
            data->op->func = cmpsb32;
        }
    }
    data->op->data1 = data->rep_zero;
    NEXT_OP(data);
    return TRUE;
}
// CMPSW
BOOL decode0a7(struct DecodeData* data) {
    if (data->ea16) {
        if (data->rep) {
            data->op->func = cmpsw16_r;
        } else {
            data->op->func = cmpsw16;
        }
    } else {
        if (data->rep) {
            data->op->func = cmpsw32_r;
        } else {
            data->op->func = cmpsw32;
        }
    }
    data->op->data1 = data->rep_zero;
    NEXT_OP(data);
    return TRUE;
}
// CMPSD
BOOL decode2a7(struct DecodeData* data) {
    if (data->ea16) {
        if (data->rep) {
            data->op->func = cmpsd16_r;
        } else {
            data->op->func = cmpsd16;
        }
    } else {
        if (data->rep) {
            data->op->func = cmpsd32_r;
        } else {
            data->op->func = cmpsd32;
        }
    }
    data->op->data1 = data->rep_zero;
    NEXT_OP(data);
    return TRUE;
}
// TEST AL,Ib
BOOL decode0a8(struct DecodeData* data) {
    data->op->func = test8_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH8(data);
    LOG_OP2("TEST", R8(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// TEST AX,Iw
BOOL decode0a9(struct DecodeData* data) {
    data->op->func = test16_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH16(data);
    LOG_OP2("TEST", R16(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// TEST EAX,Id
BOOL decode2a9(struct DecodeData* data) {
    data->op->func = test32_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH32(data);
    LOG_OP2("TEST", R32(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// STOSB
BOOL decode0aa(struct DecodeData* data) {
    if (data->ea16) {
        if (data->rep) {
            data->op->func = stosb16_r;
        } else {
            data->op->func = stosb16;
        }
    } else {
        if (data->rep) {
            data->op->func = stosb32_r;
        } else {
            data->op->func = stosb32;
        }
    }
    data->op->data1 = data->rep_zero;
    NEXT_OP(data);
    return TRUE;
}
// STOSW
BOOL decode0ab(struct DecodeData* data) {
    if (data->ea16) {
        if (data->rep) {
            data->op->func = stosw16_r;
        } else {
            data->op->func = stosw16;
        }
    } else {
        if (data->rep) {
            data->op->func = stosw32_r;
        } else {
            data->op->func = stosw32;
        }
    }
    data->op->data1 = data->rep_zero;
    NEXT_OP(data);
    return TRUE;
}
// STOSD
BOOL decode2ab(struct DecodeData* data) {
    if (data->ea16) {
        if (data->rep) {
            data->op->func = stosd16_r;
        } else {
            data->op->func = stosd16;
        }
    } else {
        if (data->rep) {
            data->op->func = stosd32_r;
        } else {
            data->op->func = stosd32;
        }
    }
    data->op->data1 = data->rep_zero;
    NEXT_OP(data);
    return TRUE;
}
// LODSB
BOOL decode0ac(struct DecodeData* data) {
    if (data->ea16) {
        if (data->rep) {
            data->op->func = lodsb16_r;
        } else {
            data->op->func = lodsb16;
        }
    } else {
        if (data->rep) {
            data->op->func = lodsb32_r;
        } else {
            data->op->func = lodsb32;
        }
    }
    data->op->data1 = data->rep_zero;
    NEXT_OP(data);
    return TRUE;
}
// LODSW
BOOL decode0ad(struct DecodeData* data) {
    if (data->ea16) {
        if (data->rep) {
            data->op->func = lodsw16_r;
        } else {
            data->op->func = lodsw16;
        }
    } else {
        if (data->rep) {
            data->op->func = lodsw32_r;
        } else {
            data->op->func = lodsw32;
        }
    }
    data->op->data1 = data->rep_zero;
    NEXT_OP(data);
    return TRUE;
}
// LODSD
BOOL decode2ad(struct DecodeData* data) {
    if (data->ea16) {
        if (data->rep) {
            data->op->func = lodsd16_r;
        } else {
            data->op->func = lodsd16;
        }
    } else {
        if (data->rep) {
            data->op->func = lodsd32_r;
        } else {
            data->op->func = lodsd32;
        }
    }
    data->op->data1 = data->rep_zero;
    NEXT_OP(data);
    return TRUE;
}
// SCASB
BOOL decode0ae(struct DecodeData* data) {
    if (data->ea16) {
        if (data->rep) {
            data->op->func = scasb16_r;
        } else {
            data->op->func = scasb16;
        }
    } else {
        if (data->rep) {
            data->op->func = scasb32_r;
        } else {
            data->op->func = scasb32;
        }
    }
    data->op->data1 = data->rep_zero;
    NEXT_OP(data);
    return TRUE;
}
// SCASW
BOOL decode0af(struct DecodeData* data) {
    if (data->ea16) {
        if (data->rep) {
            data->op->func = scasw16_r;
        } else {
            data->op->func = scasw16;
        }
    } else {
        if (data->rep) {
            data->op->func = scasw32_r;
        } else {
            data->op->func = scasw32;
        }
    }
    data->op->data1 = data->rep_zero;
    NEXT_OP(data);
    return TRUE;
}
// SCASD
BOOL decode2af(struct DecodeData* data) {
    if (data->ea16) {
        if (data->rep) {
            data->op->func = scasd16_r;
        } else {
            data->op->func = scasd16;
        }
    } else {
        if (data->rep) {
            data->op->func = scasd32_r;
        } else {
            data->op->func = scasd32;
        }
    }
    data->op->data1 = data->rep_zero;
    NEXT_OP(data);
    return TRUE;
}
// MOV AL,Ib
BOOL decode0b0(struct DecodeData* data) {
    data->op->func = mov8_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH8(data);
    LOG_OP2("MOV", R8(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV CL,Ib
BOOL decode0b1(struct DecodeData* data) {
    data->op->func = mov8_reg;
    data->op->r1 = 1;
    data->op->data1 = FETCH8(data);
    LOG_OP2("MOV", R8(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV DL,Ib
BOOL decode0b2(struct DecodeData* data) {
    data->op->func = mov8_reg;
    data->op->r1 = 2;
    data->op->data1 = FETCH8(data);
    LOG_OP2("MOV", R8(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV BL,Ib
BOOL decode0b3(struct DecodeData* data) {
    data->op->func = mov8_reg;
    data->op->r1 = 3;
    data->op->data1 = FETCH8(data);
    LOG_OP2("MOV", R8(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV AH,Ib
BOOL decode0b4(struct DecodeData* data) {
    data->op->func = mov8_reg;
    data->op->r1 = 4;
    data->op->data1 = FETCH8(data);
    LOG_OP2("MOV", R8(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV CH,Ib
BOOL decode0b5(struct DecodeData* data) {
    data->op->func = mov8_reg;
    data->op->r1 = 5;
    data->op->data1 = FETCH8(data);
    LOG_OP2("MOV", R8(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV DH,Ib
BOOL decode0b6(struct DecodeData* data) {
    data->op->func = mov8_reg;
    data->op->r1 = 6;
    data->op->data1 = FETCH8(data);
    LOG_OP2("MOV", R8(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV BH,Ib
BOOL decode0b7(struct DecodeData* data) {
    data->op->func = mov8_reg;
    data->op->r1 = 7;
    data->op->data1 = FETCH8(data);
    LOG_OP2("MOV", R8(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV AX,Iw
BOOL decode0b8(struct DecodeData* data) {
    data->op->func = mov16_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH16(data);
    LOG_OP2("MOV", R16(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV EAX,Id
BOOL decode2b8(struct DecodeData* data) {
    data->op->func = mov32_reg;
    data->op->r1 = 0;
    data->op->data1 = FETCH32(data);
    LOG_OP2("MOV", R32(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV CX,Iw
BOOL decode0b9(struct DecodeData* data) {
    data->op->func = mov16_reg;
    data->op->r1 = 1;
    data->op->data1 = FETCH16(data);
    LOG_OP2("MOV", R16(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV ECX,Id
BOOL decode2b9(struct DecodeData* data) {
    data->op->func = mov32_reg;
    data->op->r1 = 1;
    data->op->data1 = FETCH32(data);
    LOG_OP2("MOV", R32(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV DX,Iw
BOOL decode0ba(struct DecodeData* data) {
    data->op->func = mov16_reg;
    data->op->r1 = 2;
    data->op->data1 = FETCH16(data);
    LOG_OP2("MOV", R16(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV EDX,Id
BOOL decode2ba(struct DecodeData* data) {
    data->op->func = mov32_reg;
    data->op->r1 = 2;
    data->op->data1 = FETCH32(data);
    LOG_OP2("MOV", R32(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV BX,Iw
BOOL decode0bb(struct DecodeData* data) {
    data->op->func = mov16_reg;
    data->op->r1 = 3;
    data->op->data1 = FETCH16(data);
    LOG_OP2("MOV", R16(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV EBX,Id
BOOL decode2bb(struct DecodeData* data) {
    data->op->func = mov32_reg;
    data->op->r1 = 3;
    data->op->data1 = FETCH32(data);
    LOG_OP2("MOV", R32(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV SP,Iw
BOOL decode0bc(struct DecodeData* data) {
    data->op->func = mov16_reg;
    data->op->r1 = 4;
    data->op->data1 = FETCH16(data);
    LOG_OP2("MOV", R16(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV ESP,Id
BOOL decode2bc(struct DecodeData* data) {
    data->op->func = mov32_reg;
    data->op->r1 = 4;
    data->op->data1 = FETCH32(data);
    LOG_OP2("MOV", R32(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV BP,Iw
BOOL decode0bd(struct DecodeData* data) {
    data->op->func = mov16_reg;
    data->op->r1 = 5;
    data->op->data1 = FETCH16(data);
    LOG_OP2("MOV", R16(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV EBP,Id
BOOL decode2bd(struct DecodeData* data) {
    data->op->func = mov32_reg;
    data->op->r1 = 5;
    data->op->data1 = FETCH32(data);
    LOG_OP2("MOV", R32(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV SI,Iw
BOOL decode0be(struct DecodeData* data) {
    data->op->func = mov16_reg;
    data->op->r1 = 6;
    data->op->data1 = FETCH16(data);
    LOG_OP2("MOV", R16(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV ESI,Id
BOOL decode2be(struct DecodeData* data) {
    data->op->func = mov32_reg;
    data->op->r1 = 6;
    data->op->data1 = FETCH32(data);
    LOG_OP2("MOV", R32(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV DI,Iw
BOOL decode0bf(struct DecodeData* data) {
    data->op->func = mov16_reg;
    data->op->r1 = 7;
    data->op->data1 = FETCH16(data);
    LOG_OP2("MOV", R16(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// MOV EDI,Id
BOOL decode2bf(struct DecodeData* data) {
    data->op->func = mov32_reg;
    data->op->r1 = 7;
    data->op->data1 = FETCH32(data);
    LOG_OP2("MOV", R32(data->op->r1), itoa(data->op->data1, tmp, 16));
    NEXT_OP(data);
    return TRUE;
}
// RETN Iw
BOOL decode0c2(struct DecodeData* data) {
    data->op->func = retnIw16;
    data->op->data1 = FETCH16(data);
    LOG_OP1("RETN", itoa(data->op->data1, tmp, 16));
    FINISH_OP(data);
    return FALSE;
}
// RETN Iw
BOOL decode2c2(struct DecodeData* data) {
    data->op->func = retnIw32;
    data->op->data1 = FETCH16(data);
    LOG_OP1("RETN", itoa(data->op->data1, tmp, 16));
    FINISH_OP(data);
    return FALSE;
}
// RETN16
BOOL decode0c3(struct DecodeData* data) {
    data->op->func = retn16;
    LOG_OP("RETN");
    FINISH_OP(data);
    return FALSE;
}
// RETN32
BOOL decode2c3(struct DecodeData* data) {
    data->op->func = retn32;
    LOG_OP("RETN");
    FINISH_OP(data);
    return FALSE;
}
// MOV EB,IB
BOOL decode0c6(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(mov8_reg, mov8_mem16, mov8_mem32);
    data->op->data1 = FETCH8(data);
    LOG_E8C("MOV", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// MOV EW,IW
BOOL decode0c7(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(mov16_reg, mov16_mem16, mov16_mem32);
    data->op->data1 = FETCH16(data);
    LOG_E16C("MOV", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// MOV ED,ID
BOOL decode2c7(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(mov32_reg, mov32_mem16, mov32_mem32);
    data->op->data1 = FETCH32(data);
    LOG_E32C("MOV", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// LEAVE16
BOOL decode0c9(struct DecodeData* data) {
    data->op->func = leave16;
    LOG_OP("LEAVE");
    NEXT_OP(data);
    return TRUE;
}
// LEAVE32
BOOL decode2c9(struct DecodeData* data) {
    data->op->func = leave32;
    LOG_OP("LEAVE");
    NEXT_OP(data);
    return TRUE;
}
// SALC
BOOL decode0d6(struct DecodeData* data) {
    data->op->func = salc;
    LOG_OP("SALC");
    NEXT_OP(data);
    return TRUE;
}
// CALL Jw 
BOOL decode0e8(struct DecodeData* data) {
    data->op->func = callJw;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("CALL", itoa(data->op->data1, tmp, 16));
    FINISH_OP(data);
    return FALSE;
}
// CALL Jd 
BOOL decode2e8(struct DecodeData* data) {
    data->op->func = callJd;
    data->op->data1 = FETCH32(data);
    LOG_OP1("CALL", itoa(data->op->data1, tmp, 16));
    FINISH_OP(data);
    return FALSE;
}
// JMP Jw 
BOOL decode0e9(struct DecodeData* data) {
    data->op->func = jump;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JMP", itoa(data->op->data1, tmp, 16));
    FINISH_OP(data);
    return FALSE;
}
// JMP Jd 
BOOL decode2e9(struct DecodeData* data) {
    data->op->func = jump;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JMP", itoa(data->op->data1, tmp, 16));
    FINISH_OP(data);
    return FALSE;
}
// JMP Jb 
BOOL decode0eb(struct DecodeData* data) {
    data->op->func = jump;
    data->op->data1 = FETCH_S8(data);
    LOG_OP1("JMP", itoa(data->op->data1, tmp, 16));
    FINISH_OP(data);
    return FALSE;
}
// CMC
BOOL decode0f5(struct DecodeData* data) {
    data->op->func = cmc;
    LOG_OP("CMC");
    NEXT_OP(data);
    return TRUE;
}
// GRP3 Eb(,Ib)
BOOL decode0f6(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    switch (G(rm)) {
    case 0x00:
    case 0x01:
        DECODE_E(test8_reg, test8_mem16, test8_mem32);
        data->op->data1 = FETCH8(data);
        LOG_E8C("TEST", rm, data);
        break;
    case 0x02:
        DECODE_E(not8_reg, not8_mem16, not8_mem32);
        LOG_E8("NOT", rm, data);
        break;
    case 0x03:
        DECODE_E(neg8_reg, neg8_mem16, neg8_mem32);
        LOG_E8("NEG", rm, data);
        break;
    case 0x04:
        DECODE_E(mul8_reg, mul8_mem16, mul8_mem32);
        LOG_E8("MUL", rm, data);
        break;
    case 0x05:
        DECODE_E(imul8_reg, imul8_mem16, imul8_mem32);
        LOG_E8("IMUL", rm, data);
        break;
    case 0x06:
        DECODE_E(div8_reg, div8_mem16, div8_mem32);
        LOG_E8("DIV", rm, data);
        break;
    case 0x07:
        DECODE_E(idiv8_reg, idiv8_mem16, idiv8_mem32);
        LOG_E8("IDIV", rm, data);
        break;
    }
    NEXT_OP(data);
    return TRUE;
}
// GRP3 Ew(,Iw)
BOOL decode0f7(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    switch (G(rm)) {
    case 0x00:
    case 0x01:
        DECODE_E(test16_reg, test16_mem16, test16_mem32);
        data->op->data1 = FETCH16(data);
        LOG_E16C("TEST", rm, data);
        break;
    case 0x02:
        DECODE_E(not16_reg, not16_mem16, not16_mem32);
        LOG_E16("NOT", rm, data);
        break;
    case 0x03:
        DECODE_E(neg16_reg, neg16_mem16, neg16_mem32);
        LOG_E16("NEG", rm, data);
        break;
    case 0x04:
        DECODE_E(mul16_reg, mul16_mem16, mul16_mem32);
        LOG_E16("MUL", rm, data);
        break;
    case 0x05:
        DECODE_E(imul16_reg, imul16_mem16, imul16_mem32);
        LOG_E16("IMUL", rm, data);
        break;
    case 0x06:
        DECODE_E(div16_reg, div16_mem16, div16_mem32);
        LOG_E16("DIV", rm, data);
        break;
    case 0x07:
        DECODE_E(idiv16_reg, idiv16_mem16, idiv16_mem32);
        LOG_E16("IDIV", rm, data);
        break;
    }
    NEXT_OP(data);
    return TRUE;
}
// GRP3 Ed(,Id)
BOOL decode2f7(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    switch (G(rm)) {
    case 0x00:
    case 0x01:
        DECODE_E(test32_reg, test32_mem16, test32_mem32);
        data->op->data1 = FETCH32(data);
        LOG_E32C("TEST", rm, data);
        break;
    case 0x02:
        DECODE_E(not32_reg, not32_mem16, not32_mem32);
        LOG_E32("NOT", rm, data);
        break;
    case 0x03:
        DECODE_E(neg32_reg, neg32_mem16, neg32_mem32);
        LOG_E32("NEG", rm, data);
        break;
    case 0x04:
        DECODE_E(mul32_reg, mul32_mem16, mul32_mem32);
        LOG_E32("MUL", rm, data);
        break;
    case 0x05:
        DECODE_E(imul32_reg, imul32_mem16, imul32_mem32);
        LOG_E32("IMUL", rm, data);
        break;
    case 0x06:
        DECODE_E(div32_reg, div32_mem16, div32_mem32);
        LOG_E32("DIV", rm, data);
        break;
    case 0x07:
        DECODE_E(idiv32_reg, idiv32_mem16, idiv32_mem32);
        LOG_E32("IDIV", rm, data);
        break;
    }
    NEXT_OP(data);
    return TRUE;
}
// CLC
BOOL decode0f8(struct DecodeData* data) {
    data->op->func = clc;
    LOG_OP("CLC");
    NEXT_OP(data);
    return TRUE;
}
// STC
BOOL decode0f9(struct DecodeData* data) {
    data->op->func = stc;
    LOG_OP("STC");
    NEXT_OP(data);
    return TRUE;
}
// CLD
BOOL decode0fc(struct DecodeData* data) {
    data->op->func = cld;
    LOG_OP("CLD");
    NEXT_OP(data);
    return TRUE;
}
// STD
BOOL decode0fd(struct DecodeData* data) {
    data->op->func = std;
    LOG_OP("STD");
    NEXT_OP(data);
    return TRUE;
}
// RDTSC
BOOL decode131(struct DecodeData* data) {
    data->op->func = rdtsc;
    LOG_OP("RDTSC");
    NEXT_OP(data);
    return TRUE;
}
// CMOVO
BOOL decode140(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovO_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovO_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovO_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVO
BOOL decode340(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovO_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovO_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovO_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNO
BOOL decode141(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNO_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNO_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovNO_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNO
BOOL decode341(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNO_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNO_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovNO_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVB
BOOL decode142(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovB_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovB_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovB_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVB
BOOL decode342(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovB_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovB_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovB_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNB
BOOL decode143(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNB_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNB_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovNB_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNB
BOOL decode343(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNB_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNB_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovNB_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVZ
BOOL decode144(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovZ_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovZ_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovZ_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVZ
BOOL decode344(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovZ_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovZ_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovZ_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNZ
BOOL decode145(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNZ_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNZ_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovNZ_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNZ
BOOL decode345(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNZ_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNZ_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovNZ_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVBE
BOOL decode146(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovBE_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovBE_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovBE_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVBE
BOOL decode346(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovBE_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovBE_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovBE_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNBE
BOOL decode147(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNBE_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNBE_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovNBE_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNBE
BOOL decode347(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNBE_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNBE_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovNBE_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVS
BOOL decode148(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovS_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovS_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovS_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVS
BOOL decode348(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovS_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovS_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovS_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNS
BOOL decode149(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNS_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNS_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovNS_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNS
BOOL decode349(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNS_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNS_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovNS_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVP
BOOL decode14a(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovP_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovP_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovP_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVP
BOOL decode34a(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovP_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovP_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovP_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNP
BOOL decode14b(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNP_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNP_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovNP_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNP
BOOL decode34b(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNP_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNP_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovNP_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVL
BOOL decode14c(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovL_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovL_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovL_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVL
BOOL decode34c(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovL_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovL_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovL_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNL
BOOL decode14d(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNL_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNL_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovNL_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNL
BOOL decode34d(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNL_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNL_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovNL_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVLE
BOOL decode14e(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovLE_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovLE_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovLE_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVLE
BOOL decode34e(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovLE_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovLE_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovLE_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNLE
BOOL decode14f(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNLE_16_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R16(data->op->r1), R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNLE_16_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    } else {
        data->op->func = cmovNLE_16_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R16(data->op->r1), M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMOVNLE
BOOL decode34f(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmovNLE_32_reg;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("CMOV", R32(data->op->r1), R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmovNLE_32_mem16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    } else {
        data->op->func = cmovNLE_32_mem32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMOV", R32(data->op->r1), M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// JO
BOOL decode180(struct DecodeData* data) {
    data->op->func = jumpO;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JO", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNO
BOOL decode181(struct DecodeData* data) {
    data->op->func = jumpNO;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JNO", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JB
BOOL decode182(struct DecodeData* data) {
    data->op->func = jumpB;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JB", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNB
BOOL decode183(struct DecodeData* data) {
    data->op->func = jumpNB;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JNB", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JZ
BOOL decode184(struct DecodeData* data) {
    data->op->func = jumpZ;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JZ", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNZ
BOOL decode185(struct DecodeData* data) {
    data->op->func = jumpNZ;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JNZ", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JBE
BOOL decode186(struct DecodeData* data) {
    data->op->func = jumpBE;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JBE", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNBE
BOOL decode187(struct DecodeData* data) {
    data->op->func = jumpNBE;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JNBE", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JS
BOOL decode188(struct DecodeData* data) {
    data->op->func = jumpS;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JS", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNS
BOOL decode189(struct DecodeData* data) {
    data->op->func = jumpNS;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JNS", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JP
BOOL decode18a(struct DecodeData* data) {
    data->op->func = jumpP;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JP", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNP
BOOL decode18b(struct DecodeData* data) {
    data->op->func = jumpNP;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JNP", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JL
BOOL decode18c(struct DecodeData* data) {
    data->op->func = jumpL;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JL", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNL
BOOL decode18d(struct DecodeData* data) {
    data->op->func = jumpNL;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JNL", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JLE
BOOL decode18e(struct DecodeData* data) {
    data->op->func = jumpLE;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JLE", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNLE
BOOL decode18f(struct DecodeData* data) {
    data->op->func = jumpNLE;
    data->op->data1 = FETCH_S16(data);
    LOG_OP1("JNLE", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JO
BOOL decode380(struct DecodeData* data) {
    data->op->func = jumpO;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JO", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNO
BOOL decode381(struct DecodeData* data) {
    data->op->func = jumpNO;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JNO", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JB
BOOL decode382(struct DecodeData* data) {
    data->op->func = jumpB;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JB", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNB
BOOL decode383(struct DecodeData* data) {
    data->op->func = jumpNB;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JNB", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JZ
BOOL decode384(struct DecodeData* data) {
    data->op->func = jumpZ;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JZ", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNZ
BOOL decode385(struct DecodeData* data) {
    data->op->func = jumpNZ;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JNZ", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JBE
BOOL decode386(struct DecodeData* data) {
    data->op->func = jumpBE;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JBE", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNBE
BOOL decode387(struct DecodeData* data) {
    data->op->func = jumpNBE;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JNBE", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JS
BOOL decode388(struct DecodeData* data) {
    data->op->func = jumpS;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JS", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNS
BOOL decode389(struct DecodeData* data) {
    data->op->func = jumpNS;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JNS", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JP
BOOL decode38a(struct DecodeData* data) {
    data->op->func = jumpP;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JP", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNP
BOOL decode38b(struct DecodeData* data) {
    data->op->func = jumpNP;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JNP", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JL
BOOL decode38c(struct DecodeData* data) {
    data->op->func = jumpL;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JL", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNL
BOOL decode38d(struct DecodeData* data) {
    data->op->func = jumpNL;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JNL", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JLE
BOOL decode38e(struct DecodeData* data) {
    data->op->func = jumpLE;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JLE", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// JNLE
BOOL decode38f(struct DecodeData* data) {
    data->op->func = jumpNLE;
    data->op->data1 = FETCH32(data);
    LOG_OP1("JNLE", itoa((int)data->op->data1, tmp, 10));
    FINISH_OP(data);
    return FALSE;
}
// SETO
BOOL decode190(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setO_reg, setO_mem16, setO_mem32);
    LOG_E8("SETO", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// SETNO
BOOL decode191(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setNO_reg, setNO_mem16, setNO_mem32);
    LOG_E8("SETNO", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// SETB
BOOL decode192(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setB_reg, setB_mem16, setB_mem32);
    LOG_E8("SETB", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// SETNB
BOOL decode193(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setNB_reg, setNB_mem16, setNB_mem32);
    LOG_E8("SETNB", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// SETZ
BOOL decode194(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setZ_reg, setZ_mem16, setZ_mem32);
    LOG_E8("SETZ", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// SETNZ
BOOL decode195(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setNZ_reg, setNZ_mem16, setNZ_mem32);
    LOG_E8("SETNZ", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// SETBE
BOOL decode196(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setBE_reg, setBE_mem16, setBE_mem32);
    LOG_E8("SETBE", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// SETNBE
BOOL decode197(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setNBE_reg, setNBE_mem16, setNBE_mem32);
    LOG_E8("SETNBE", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// SETS
BOOL decode198(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setS_reg, setS_mem16, setS_mem32);
    LOG_E8("SETS", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// SETNS
BOOL decode199(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setNS_reg, setNS_mem16, setNS_mem32);
    LOG_E8("SETNS", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// SETP
BOOL decode19a(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setP_reg, setP_mem16, setP_mem32);
    LOG_E8("SETP", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// SETNP
BOOL decode19b(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setNP_reg, setNP_mem16, setNP_mem32);
    LOG_E8("SETNP", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// SETL
BOOL decode19c(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setL_reg, setL_mem16, setL_mem32);
    LOG_E8("SETL", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// SETNL
BOOL decode19d(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setNL_reg, setNL_mem16, setNL_mem32);
    LOG_E8("SETNL", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// SETLE
BOOL decode19e(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setLE_reg, setLE_mem16, setLE_mem32);
    LOG_E8("SETLE", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// SETNLE
BOOL decode19f(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    DECODE_E(setNLE_reg, setNLE_mem16, setNLE_mem32);
    LOG_E8("SETNLE", rm, data);
    NEXT_OP(data);
    return TRUE;
}
// POP FS
BOOL decode1a1(struct DecodeData* data) {
    data->op->func = popSeg16;
    data->op->r1 = FS;
    LOG_OP("POP FS");
    NEXT_OP(data);
    return TRUE;
}
// POP FS
BOOL decode3a1(struct DecodeData* data) {
    data->op->func = popSeg32;
    data->op->r1 = FS;
    LOG_OP("POP FS");
    NEXT_OP(data);
    return TRUE;
}
// CPUID
BOOL decode1a2(struct DecodeData* data) {
    data->op->func = cpuid;
    LOG_OP("CPUID");
    NEXT_OP(data);
    return TRUE;
}
// BT Ew,Gw
BOOL decode1a3(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = btr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("BT", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = bte16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("BT", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = bte16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("BT", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// BT Ed,Gd
BOOL decode3a3(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = btr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("BT", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = bte32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("BT", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = bte32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("BT", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// DSHL Ew,Gw
BOOL decode1a4(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = dshlr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("DSHL", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = dshle16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("DSHL", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = dshle16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("DSHL", M16(data, rm, data->op),R16(data->op->r1));
    }
    data->op->data1 = FETCH8(data);
    data->op->data1 &= 0x1f;
    if (data->op->data1 == 0) {
        RESTART(data);
        return TRUE;
    }
    NEXT_OP(data);
    return TRUE;
}
// DSHL Ed,Gd
BOOL decode3a4(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = dshlr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("DSHL", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = dshle32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("DSHL", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = dshle32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("DSHL", M32(data, rm, data->op),R32(data->op->r1));
    }
    data->op->data1 = FETCH8(data);
    data->op->data1 &= 0x1f;
    if (data->op->data1 == 0) {
        RESTART(data);
        return TRUE;
    }
    NEXT_OP(data);
    return TRUE;
}
// DSHLCL Ew,Gw
BOOL decode1a5(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = dshlclr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("DSHLCL", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = dshlcle16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("DSHLCL", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = dshlcle16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("DSHLCL", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// DSHLCL Ed,Gd
BOOL decode3a5(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = dshlclr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("DSHLCL", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = dshlcle32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("DSHLCL", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = dshlcle32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("DSHLCL", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// POP GS
BOOL decode1a9(struct DecodeData* data) {
    data->op->func = popSeg16;
    data->op->r1 = GS;
    LOG_OP("POP GS");
    NEXT_OP(data);
    return TRUE;
}
// POP GS
BOOL decode3a9(struct DecodeData* data) {
    data->op->func = popSeg32;
    data->op->r1 = GS;
    LOG_OP("POP GS");
    NEXT_OP(data);
    return TRUE;
}
// BTS Ew,Gw
BOOL decode1ab(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = btsr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("BTS", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = btse16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("BTS", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = btse16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("BTS", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// BTS Ed,Gd
BOOL decode3ab(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = btsr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("BTS", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = btse32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("BTS", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = btse32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("BTS", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// DSHR Ew,Gw
BOOL decode1ac(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = dshrr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("DSHR", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = dshre16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("DSHR", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = dshre16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("DSHR", M16(data, rm, data->op),R16(data->op->r1));
    }
    data->op->data1 = FETCH8(data);
    data->op->data1 &= 0x1f;
    if (data->op->data1 == 0) {
        RESTART(data);
        return TRUE;
    }
    NEXT_OP(data);
    return TRUE;
}
// DSHR Ed,Gd
BOOL decode3ac(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = dshrr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("DSHR", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = dshre32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("DSHR", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = dshre32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("DSHR", M32(data, rm, data->op),R32(data->op->r1));
    }
    data->op->data1 = FETCH8(data);
    data->op->data1 &= 0x1f;
    if (data->op->data1 == 0) {
        RESTART(data);
        return TRUE;
    }
    NEXT_OP(data);
    return TRUE;
}
// DSHRCL Ew,Gw
BOOL decode1ad(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = dshrclr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("DSHRCL", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = dshrcle16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("DSHRCL", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = dshrcle16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("DSHRCL", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// DSHRCL Ed,Gd
BOOL decode3ad(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = dshrclr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("DSHRCL", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = dshrcle32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("DSHRCL", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = dshrcle32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("DSHRCL", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// DIMUL Gw,Ew
BOOL decode1af(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = dimulr16r16;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("DIMUL", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = dimulr16e16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("DIMUL", R16(data->op->r1),M16(data, rm, data->op));
    } else {
        data->op->func = dimulr16e16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("DIMUL", R16(data->op->r1),M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// DIMUL Gd,Ed
BOOL decode3af(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = dimulr32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("DIMUL", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = dimulr32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("DIMUL", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = dimulr32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("DIMUL", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMPXCHG Ew,Gw
BOOL decode1b1(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmpxchgr16r16;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("CMPXCHG", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmpxchge16r16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMPXCHG", M16(data, rm, data->op),R16(data->op->r1));
    } else {
        data->op->func = cmpxchge16r16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMPXCHG", M16(data, rm, data->op),R16(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// CMPXCHG Ed,Gd
BOOL decode3b1(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = cmpxchgr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("CMPXCHG", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = cmpxchge32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("CMPXCHG", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = cmpxchge32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("CMPXCHG", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// MOVXZ8 Gw,Ew
BOOL decode1b6(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = movxz8r16r16;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("MOVXZ8", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = movxz8r16e16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("MOVXZ8", R16(data->op->r1),M16(data, rm, data->op));
    } else {
        data->op->func = movxz8r16e16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("MOVXZ8", R16(data->op->r1),M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// MOVXZ8 Gd,Ed
BOOL decode3b6(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = movxz8r32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("MOVXZ8", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = movxz8r32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("MOVXZ8", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = movxz8r32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("MOVXZ8", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// MOVXZ16 Gd,Ed
BOOL decode3b7(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = movxz16r32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("MOVXZ16", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = movxz16r32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("MOVXZ16", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = movxz16r32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("MOVXZ16", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// BTC Ed,Gd
BOOL decode3bb(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = btcr32r32;
        data->op->r1 = E(rm);
        data->op->r2 = G(rm);
        LOG_OP2("BTC", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = btce32r32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("BTC", M32(data, rm, data->op),R32(data->op->r1));
    } else {
        data->op->func = btce32r32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("BTC", M32(data, rm, data->op),R32(data->op->r1));
    }
    NEXT_OP(data);
    return TRUE;
}
// BSF Gd,Ed
BOOL decode3bc(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = bsfr32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("BSF", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = bsfr32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("BSF", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = bsfr32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("BSF", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// BSR Gw,Ew
BOOL decode1bd(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = bsrr16r16;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("BSR", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = bsrr16e16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("BSR", R16(data->op->r1),M16(data, rm, data->op));
    } else {
        data->op->func = bsrr16e16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("BSR", R16(data->op->r1),M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// BSR Gd,Ed
BOOL decode3bd(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = bsrr32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("BSR", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = bsrr32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("BSR", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = bsrr32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("BSR", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// MOVSX8 Gw,Ew
BOOL decode1be(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = movsx8r16r16;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("MOVSX8", R16(data->op->r1),R16(data->op->r2));
    } else if (data->ea16) {
        data->op->func = movsx8r16e16_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("MOVSX8", R16(data->op->r1),M16(data, rm, data->op));
    } else {
        data->op->func = movsx8r16e16_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("MOVSX8", R16(data->op->r1),M16(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// MOVSX8 Gd,Ed
BOOL decode3be(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = movsx8r32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("MOVSX8", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = movsx8r32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("MOVSX8", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = movsx8r32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("MOVSX8", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
// MOVSX16 Gd,Ed
BOOL decode3bf(struct DecodeData* data) {
    U8 rm = FETCH8(data);
    if (rm>=0xC0) {
        data->op->func = movsx16r32r32;
        data->op->r1 = G(rm);
        data->op->r2 = E(rm);
        LOG_OP2("MOVSX16", R32(data->op->r1),R32(data->op->r2));
    } else if (data->ea16) {
        data->op->func = movsx16r32e32_16;
        data->op->r1 = G(rm);
        decodeEa16(data, rm);
        LOG_OP2("MOVSX16", R32(data->op->r1),M32(data, rm, data->op));
    } else {
        data->op->func = movsx16r32e32_32;
        data->op->r1 = G(rm);
        decodeEa32(data, rm);
        LOG_OP2("MOVSX16", R32(data->op->r1),M32(data, rm, data->op));
    }
    NEXT_OP(data);
    return TRUE;
}
