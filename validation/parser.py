#!/usr/bin/env python

import os
import math
from geantval import energy_mev, getJSON, one_command

# Gamma processes in this application; a macro measures the attenuation of the
# ones left active by /process/inactivate <name> gamma (as the legacy includes).
PROCESSES = {"phot": "photo", "compt": "compton", "conv": "conv", "Rayl": "ray"}


def extractObservable(filename):
    x = []
    y = []
    with open(filename) as myfile:
        for line in myfile:
            line = line.strip()
            if line and not line.startswith(("energy", "#")):
                line=line.split()
                if len(line) != 2:
                    raise ValueError("Expected energy/value columns")
                x.append(float(line[0]))
                y.append(float(line[1]))
    if not x:
        raise ValueError("Empty attenuation output")
    return (x, y)


def parse(job):
    filepath = os.path.join(job["path"], "AttenuationCoefficient.out")
    print(filepath)
    xvalues, yvalues = extractObservable(filepath)
    if len(xvalues) != job["EXPECTED_POINTS"]:
        raise ValueError("Incomplete attenuation energy scan")
    if any(not math.isclose(x, e, rel_tol=1e-5) for x, e in zip(xvalues, job["ENERGIES"])):
        raise ValueError("Attenuation energies do not match the macro")

    rjson = getJSON(job, "chart",
                    mctool_name="GEANT4",
                    mctool_model=job["PHYSLIST"],
                    observableName="attenuation coefficient(" + job["PROCESS"] + ")",
                    targetName=job["MATERIAL"].split("_")[1],
                    beamParticle=job["PARTICLE"],
                    beamEnergies=xvalues[:3],
                    secondaryParticle="None",
                    title="attenuation coefficient",
                    xAxisName="energy, MeV",
                    yAxisName="attenuation coefficient, cm^{2}/g",
                    xValues=xvalues,
                    yValues=yvalues
                    )
    yield rjson


def metadata(commands):
    energies = []
    energy = None
    inactive = set()
    for command, value in commands:
        if command == "/gun/energy":
            energy = energy_mev(value)
        if command == "/process/inactivate":
            name, *particle = value.split()
            if particle != ["gamma"] or name not in PROCESSES:
                raise ValueError("Unexpected process selection: " + value)
            inactive.add(name)
        if command == "/run/beamOn":
            if energy is None or int(value) <= 0:
                raise ValueError("Each beamOn needs an explicit energy and positive event count")
            energies.append(energy)
    if not energies:
        raise ValueError("No energy scan in macro")
    return {"TEST": "attenuation",
            "PHYSLIST": one_command(commands, "/testem/phys/addPhysics"),
            "MATERIAL": one_command(commands, "/testem/det/setMat", "G4_WATER"),
            "PARTICLE": one_command(commands, "/gun/particle"),
            "PROCESS": process(inactive),
            "EXPECTED_POINTS": len(energies), "ENERGIES": energies}


def process(inactive):
    active = set(PROCESSES) - inactive
    if active == set(PROCESSES):
        return "total"
    if len(active) != 1:
        raise ValueError("Leave all gamma processes or exactly one active")
    return PROCESSES[active.pop()]
