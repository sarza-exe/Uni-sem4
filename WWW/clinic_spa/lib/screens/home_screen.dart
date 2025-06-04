// lib/screens/home_screen.dart
import 'package:flutter/material.dart';
import '../services/api_service.dart';
import 'login_screen.dart';

class HomeScreen extends StatelessWidget {
  const HomeScreen({Key? key}) : super(key: key);

  Future<void> _logout(BuildContext context) async {
    await ApiService().logout(); // Implement logout to delete JWT from storage
    if (!context.mounted) return;
    Navigator.pushReplacement(
      context,
      MaterialPageRoute(builder: (_) => const LoginScreen()),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Clinic Dashboard'),
      ),
      drawer: Drawer(
        child: ListView(
          padding: EdgeInsets.zero,
          children: [
            const DrawerHeader(
              decoration: BoxDecoration(color: Colors.blue),
              child: Text(
                'Clinic Menu',
                style: TextStyle(color: Colors.white, fontSize: 24),
              ),
            ),
            ListTile(
              leading: const Icon(Icons.medical_services),
              title: const Text('Doctors'),
              onTap: () => Navigator.pushNamed(context, '/doctors'),
            ),
            ListTile(
              leading: const Icon(Icons.people),
              title: const Text('Patients'),
              onTap: () => Navigator.pushNamed(context, '/patients'),
            ),
            ListTile(
              leading: const Icon(Icons.event_note),
              title: const Text('Appointments'),
              onTap: () => Navigator.pushNamed(context, '/appointments'),
            ),
            const Divider(),
            ListTile(
              leading: const Icon(Icons.logout),
              title: const Text('Logout'),
              onTap: () => _logout(context),
            ),
          ],
        ),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text(
              'Welcome to Clinic App',
              style: Theme.of(context).textTheme.titleLarge,
            ),
            const SizedBox(height: 24),
            ElevatedButton.icon(
              icon: const Icon(Icons.medical_services),
              label: const Text('View Doctors'),
              onPressed: () => Navigator.pushNamed(context, '/doctors'),
              style: ElevatedButton.styleFrom(minimumSize: const Size(200, 48)),
            ),
            const SizedBox(height: 16),
            ElevatedButton.icon(
              icon: const Icon(Icons.people),
              label: const Text('View Patients'),
              onPressed: () => Navigator.pushNamed(context, '/patients'),
              style: ElevatedButton.styleFrom(minimumSize: const Size(200, 48)),
            ),
            const SizedBox(height: 16),
            ElevatedButton.icon(
              icon: const Icon(Icons.event_note),
              label: const Text('View Appointments'),
              onPressed: () => Navigator.pushNamed(context, '/appointments'),
              style: ElevatedButton.styleFrom(minimumSize: const Size(200, 48)),
            ),
          ],
        ),
      ),
    );
  }
}
