void resetAvailability() {
    for (auto &kv : cabs) kv.second.available = true;
    activeRides.clear();  // Important: remove any active rides
    saveCabs();
    cout << "All cabs set to available.\n";
}