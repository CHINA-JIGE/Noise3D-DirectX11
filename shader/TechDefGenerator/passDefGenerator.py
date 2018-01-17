vsFunc = 'VS_DrawMeshWithPixelLighting'
psFunc = 'PS_DrawMeshWithPixelLighting'
effectSwitchCount = 4

with open('PassDefinition.txt', 'w') as f:
    # 2^n combinations in total for n effect switches
    for passID in range(0, pow(2, effectSwitchCount)):
        f.write('\tpass perPixel_' + str(passID) + '\n')
        f.write('\t{\n')
        f.write('\t\tSetVertexShader(CompileShader(vs_5_0, ' + str(vsFunc) + '()));\n')
        f.write('\t\tSetGeometryShader(NULL);\n')
        f.write('\t\tSetPixelShader(CompileShader(ps_5_0, ' + str(psFunc) + '(')
        # switch combination
        for switchID in range(0, effectSwitchCount):
            # to write all switches to "true"/"false"
            isTurnedOn = "true" if passID & (1 << switchID) else "false"
            f.write(isTurnedOn)
            if switchID != effectSwitchCount-1:
                f.write(', ')
            else:
                f.write(')')
        # all switches written in a pass
        f.write('));\n')
        f.write("\t}\n")




