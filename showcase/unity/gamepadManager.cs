using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

public class gamepadManager : MonoBehaviour
{
    public static gamepadManager instance { get; set; }

    GamepadInput gamepadInput;

    [Range(-1, 1)]
    public float xAxis, yAxis, zAxis;

    [Space(20)]

    [Range(-1, 1)]
    public float RxAxis;
    [Range(-1, 1)]
    public float RyAxis, RzAxis;

    private void Awake()
    {
        instance = this;
        gamepadInput = new GamepadInput();

        gamepadInput.gamepad.xAxis.performed += context => { xAxis = context.ReadValue<float>(); };
        gamepadInput.gamepad.yAxis.performed += context => { yAxis = context.ReadValue<float>(); };
        gamepadInput.gamepad.zAxis.performed += context => { zAxis = context.ReadValue<float>(); };

        gamepadInput.gamepad.RxAxis.performed += context => { RxAxis = context.ReadValue<float>(); };
        gamepadInput.gamepad.RyAxis.performed += context => { RyAxis = -context.ReadValue<float>(); };
        gamepadInput.gamepad.RzAxis.performed += context => { RzAxis = context.ReadValue<float>(); };
    }

    void Update()
    {
        // Sürekli olarak eksen değerlerini kontrol et
        xAxis = gamepadInput.gamepad.xAxis.ReadValue<float>();
        yAxis = gamepadInput.gamepad.yAxis.ReadValue<float>();
        zAxis = gamepadInput.gamepad.zAxis.ReadValue<float>();

        RxAxis = gamepadInput.gamepad.RxAxis.ReadValue<float>();
        RyAxis = -gamepadInput.gamepad.RyAxis.ReadValue<float>();
        RzAxis = gamepadInput.gamepad.RzAxis.ReadValue<float>();
    }

    private void OnEnable()
    {
        gamepadInput.gamepad.Enable();
    }
    private void OnDisable()
    {
        gamepadInput.gamepad.Enable();
    }
}
