Class.refactor(
    Drag,
    {
        attach: function ()
        {
            this.handles.addEvent('touchstart', this.bound.start);
            return this.previous.apply(this, arguments);
        },

        detach: function ()
        {
            this.handles.removeEvent('touchstart', this.bound.start);
            return this.previous.apply(this, arguments);
        },

        start: function (event)
        {
            document.body.addEvents(
                {
                    touchmove: this.bound.check,
                    touchmove: this.bound.check,
                    touchmove: this.bound.check,
                    touchend: this.bound.cancel
                }
            );
            this.previous.apply(this, arguments);
        },

        check: function (event)
        {
            if (this.options.preventDefault) {
                event.preventDefault();
            }
            var distance = Math.round(Math.sqrt(Math.pow(event.page.x - this.mouse.start.x, 2) + Math.pow(event.page.y - this.mouse.start.y, 2)));
            if (distance > this.options.snap) {
                this.cancel();
                this.document.addEvents(
                    {
                        mousemove: this.bound.drag,
                        mouseup: this.bound.stop
                    }
                );
                document.body.addEvents(
                    {
                        touchmove: this.bound.drag,
                        touchend: this.bound.stop
                    }
                );
                this.fireEvent('start', [this.element, event]).fireEvent('snap', this.element);
            }
        },

        cancel: function (event)
        {
            document.body.removeEvents(
                {
                    touchmove: this.bound.check,
                    touchend: this.bound.cancel
                }
            );
            return this.previous.apply(this, arguments);
        },

        stop: function (event)
        {
            document.body.removeEvents(
                {
                    touchmove: this.bound.drag,
                    touchend: this.bound.stop
                }
            );
            return this.previous.apply(this, arguments);
        }
    }
);

var Controller = new Class({

    lastSpeed: 0,
    lastDirection: 0,
    angle: null,

    initialize: function(name){
        this.request = new Request.JSON({url: 'api.php',onSuccess: this.requestSuccess.bind(this)});
        this.pimobileRequest = new Request.JSON();
        this.slider = document.id('slider');
        this.controller = document.id('controller');
        this.compassRose = document.id('compassRose');
        this.compassDigital = document.id('compassDigital');
        this.distanceDigital = document.id('distanceDigital');
        this.distanceVisual = document.id('distanceVisual');
        this.cam = document.id('cam');

        var rect = this.slider.getBoundingClientRect();
        this.sliderWidth = rect.width;
        this.sliderHeight = rect.height;

        this.drag = new Drag(
            this.slider, {
                snap: 5,
                stopPropagation: true,
                preventDefault: true
            }
        );

        this.drag.addEvent('complete',function(el) {
            el.addClass('moveTransition');
            el.setStyle('left','calc(50% - 3rem)');
            el.setStyle('top','calc(50% - 3rem)');
            // stop motors
            this.request.cancel();
            this.sendMotorData(0,0);
        }.bind(this));

        this.drag.addEvent('drag:throttle(50)',function(el) {
            el.removeClass('moveTransition');
            this.update();
        }.bind(this));


        // get camera feed url
        this.cameraUrl = this.cam.getStyle('background-image').replace(/url\(http/, 'url("http').replace(/"\)/, '') + '&v=';
        // reload camera image by adding random cache parameter
        this.reloadCameraImage();

        this.adjustControllerSize();
        window.addEvent('resize:throttle(500)', this.adjustControllerSize.bind(this));
        this.controller.getElement('button').addEvent('click', this.toggleCamera.bind(this));

        // light switch
        document.id('lightSwitch').addEvent('click', this.toggleLight.bind(this));

        // compassRose feed
        this.sensorRequestCompass = new Request.JSON({
            method: 'get',
            url: '/compass',
            initialDelay: 1000,
            delay: 200,
            limit: 15000,
            onSuccess: this.compassCallback.bind( this )
        }).startTimer();
        this.sensorRequestUltra = new Request.JSON({
            method: 'get',
            url: '/ultra',
            initialDelay: 1100,
            delay: 200,
            limit: 15000,
            onSuccess: this.ultraCallback.bind( this )
        }).startTimer();
    },

    compassCallback: function(responseJSON){
        if(Math.abs(this.angle-responseJSON.angle) < 1) {
            // only update compassRose on relevant position changes
            return;
        }

        // turn off css transition when moving from >=0° to <0° or the other way around to avoid a full spin of compassRose
        if(this.angle !== null && ( ( this.angle > 300 && responseJSON.angle < 90 ) || ( this.angle < 90 && responseJSON.angle > 300 ) )) {
        this.compassRose.addClass('noTransition');
        } else {
            this.compassRose.removeClass('noTransition');
        }
        this.compassRose.setStyle('transform','rotate(' + (-1*responseJSON.angle) + 'deg)');
        this.angle = responseJSON.angle;
        this.compassDigital.innerText = this.angle + '°';
    },

    ultraCallback: function(responseJSON){
        if(responseJSON.error || responseJSON.distance == 0) {
            return;
        }
        this.distanceDigital.innerText = responseJSON.distance + ' cm';
        this.distanceVisual.setStyle('height', (( responseJSON.distance / 200 ) * 100) + '%' );
    },

    requestSuccess: function(response) {
        if(response.reloadCameraImage) {
            // force refresh image
            this.reloadCameraImage.delay(1700,this);
        }
    },

    reloadCameraImage: function() {
        this.cam.setStyle('background-image',this.cameraUrl + Math.floor(Math.random()*1000000) + '")');
    },

    toggleLight: function() {
        this.pimobileRequest.send({url: '/pimobile/frontlight'});
    },

    toggleCamera: function() {
        this.cam.toggleClass('noCamera');
        this.controller.toggleClass('noCamera');
        if(this.cam.hasClass('noCamera')){
            // turn off camera stream via api
            this.request.put({'camera': 0});
            // turn off sensor feed
            this.sensorRequestCompass.stopTimer();
            this.sensorRequestUltra.stopTimer();
        } else {
            // set loading background
            this.cam.setStyle('background-image','url("/images/371.GIF")');
            // turn on camera stream via api
            this.request.put({'camera': 1});
            // turn on sensor feed
            this.sensorRequestCompass.startTimer();
            this.sensorRequestUltra.startTimer();
        }
        // adjust controller size, use delay due to transition time
        this.adjustControllerSize.delay(900,this);
    },

    adjustControllerSize: function() {
        var rect = this.controller.getBoundingClientRect();
        this.viewPortWidth = rect.width - this.sliderWidth;
        this.viewPortHeight = rect.height - this.sliderHeight;
        this.drag.options.limit = {'x': [0,this.viewPortWidth], 'y': [0,this.viewPortHeight]};
    },

    sendMotorData: function( speed, direction ) {
        if(this.pimobileRequest.isRunning()) {
//            this.request.cancel();
            return;
        }

        if(speed != this.lastSpeed || direction != this.lastDirection) {
            this.lastSpeed = speed;
            this.lastDirection = direction;
//        console.log(speed, direction);
            this.pimobileRequest.send({url: '/pimobile/speed/' + speed + '/' + direction});
        }
    },

    update: function() {
        // calculate speed and direction
        var top = this.slider.getStyle('top').replace(/px/,'');
        var left = this.slider.getStyle('left').replace(/px/,'');
        var speed = Math.floor( ( top / this.viewPortHeight ) * -200 + 100 );
        var direction = Math.floor( ( left / this.viewPortWidth ) * 200 - 100 );
        this.sendMotorData(speed, direction);
//        console.log(left,top);
    }
});

window.addEvent(
    'domready', function () {
        new Controller();
    }
);