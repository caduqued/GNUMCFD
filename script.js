document.addEventListener('DOMContentLoaded', function () {
    // Menú y navegación desplegable
    let menu = document.querySelector('#menu-icon');
    let navlist = document.querySelector('.navlist');

    menu.onclick = () => {
        menu.classList.toggle('bx-x');
        navlist.classList.toggle('open');
    };

    // Configuración de ScrollReveal
    const sr = ScrollReveal({
        distance: '100px',
        duration: 2600,
        delay: 450,
        reset: true
    });

    sr.reveal('.principal_logo', { delay: 200, origin: 'top' });
    sr.reveal('.description', { delay: 450, origin: 'top' });
    sr.reveal('.navlist', { delay: 550, origin: 'top', reset: false });
    sr.reveal('header .logo', { delay: 550, origin: 'top', reset: false });
    sr.reveal('header', { delay: 550, origin: 'top', reset: false });
    sr.reveal('.icons', { delay: 500, origin: 'right' });

    let counter = 0;
    const slider = document.querySelector('.slider');
    const slides_img = document.querySelectorAll('.slider img');
    const slides_video = document.querySelectorAll('.slider video');
    const slides = [...slides_img, ...slides_video]; // Combina las imágenes y los videos en un solo array
    const interval = 5000; // tiempo en milisegundos
    
    function autoSlide() {
        slider.scrollLeft += slides[counter].offsetWidth;
        counter += 1;
    
        if (counter >= slides.length) {
            slider.scrollLeft = 0;
            counter = 0;
        }
    }
    
    setInterval(autoSlide, interval);
    



});
