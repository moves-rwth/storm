Matrix3i m = Matrix3i::Random();
cout << "Here is the matrix m:" << endl << m << endl;
cout << "Here is the upper-triangular matrix extracted from m:" << endl
     << Matrix3i(m.triangularView<StormEigen::Upper>()) << endl;
cout << "Here is the strictly-upper-triangular matrix extracted from m:" << endl
     << Matrix3i(m.triangularView<StormEigen::StrictlyUpper>()) << endl;
cout << "Here is the unit-lower-triangular matrix extracted from m:" << endl
     << Matrix3i(m.triangularView<StormEigen::UnitLower>()) << endl;
// FIXME need to implement output for triangularViews (Bug 885)
