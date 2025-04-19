// plik: js/nav.js
document.addEventListener('DOMContentLoaded', () => {
    const nav = document.getElementById('topnav');
    const links = Array.from(nav.querySelectorAll('a'));
    
    // 1) Tworzymy przycisk hamburger
    const btn = document.createElement('button');
    btn.id = 'hamburger';
    btn.setAttribute('aria-label', 'Menu');
    btn.innerHTML = '&#9776;'; // ☰
    nav.insertBefore(btn, nav.firstChild);
  
    // 2) Funkcja przełączająca widoczność linków
    function toggleMenu() {
      nav.classList.toggle('open');
    }
    btn.addEventListener('click', toggleMenu);
  
    // 3) Obsługa zmiany rozmiaru
    function handleResize() {
      if (window.innerWidth <= 600) {
        btn.style.display = 'block';
        if (!nav.classList.contains('open')) {
          links.forEach(l => l.style.display = 'none');
        }
      } else {
        btn.style.display = 'none';
        nav.classList.remove('open');
        links.forEach(l => l.style.display = 'inline-block');
      }
    }
    window.addEventListener('resize', handleResize);
  
    // 4) Inicjalne ustawienie
    handleResize();
  });
  