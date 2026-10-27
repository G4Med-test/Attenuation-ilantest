# Portal JSON parser

Adapted from `geant-config-generator/tests/geant4/attenuation/parser.py` at commit
`0bf42953c9c98ac6ecccf077e7c7fe2c4ceb7307`.

`config.json` selects the total attenuation scan with Livermore physics. Only this
production macro is selected; `unit.mac`, `vis.mac` and the additional `.in` scans
are not run. Add further self-contained scans explicitly to `config.json`. The
measured process is derived from the macro: `total` when no gamma process is
inactivated, otherwise the single process left active by
`/process/inactivate <phot|compt|conv|Rayl> gamma` (as in the legacy includes).

The parser reads `AttenuationCoefficient.out`, checks its rows and energies against
all `/run/beamOn` commands, and derives the remaining metadata from the macro.
The default material is G4_WATER, matching DetectorConstruction. Nonfinite values
are rejected by the exporter. The legacy `SCATTER2D` schema, `testName: attenuation`,
empty uncertainty arrays and first-three-energies metadata convention are preserved;
all energies are included in `chart.xValues`.

The pipeline uploads `validation-json/results.json` (an array of plot objects)
and `plots/*.json` (the original one-object-per-file format), plus raw artifacts.
See [ci-workflows](https://github.com/G4Med-test/ci-workflows) for local CLI usage.
